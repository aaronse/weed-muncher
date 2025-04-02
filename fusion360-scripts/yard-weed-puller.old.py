import adsk.core, adsk.fusion, adsk.cam
import traceback
import json

# ------------------------------------------------
# Global “magic numbers” or defaults
# You can later load these from a JSON config.
# ------------------------------------------------
PARAMS = {
    "EMT": {
        "outerDiameter_mm": 17.0,  # approximate OD for 1/2" EMT
        "innerDiameter_mm": 15.5,  # approximate ID for 1/2" EMT
        "length_mm": 1219.2,       # ~48 inches in mm
    },
    "Gripper": {
        "jawSpan_mm": 50.0,        # ~2" or smaller
        "armThickness_mm": 3.0,
        "armLength_mm": 80.0,
        "tpuPadThickness_mm": 2.5,
    },
    "Lever": {
        "leverPivotHeight_mm": 1016.0,  # ~40" from bottom
        "handleLength_mm": 100.0,
        "triggerOffset_mm": 30.0
    },
    "Hardware": {
        "commonBoltDiameter_mm": 5.0,
        "commonBoltLength_mm": 30.0
    },
    "Belts": {
        "useGT2Belt": True,
        "beltWidth_mm": 10.0,
        "pulleyDiameter_mm": 16.0
    },
    "FilamentSpool": {
        "outerDiameter_mm": 200.0,
        "rimThickness_mm": 3.0,
        "overallWidth_mm": 65.0,
        "hubOuterDiameter_mm": 80.0,
        "hubInnerDiameter_mm": 55.0
    },
    "SpoolMount": {
        "cClampWallThickness_mm": 3.0,    # PETG clamp thickness
        "cClampClearance_mm": 1.0,       # clearance so spool spins
        "grooveDepth_mm": 0.5,           # height of the friction-reducing ridges
        "grooveCount": 4,               # how many grooves around the clamp
        "spoolOffsetFromBottom_mm": 50.0 # how far up from the bottom of EMT
    },
    # ... Add other global parameters as needed ...
}


# ------------------------------------------------
# Entry point for Fusion 360 add-in / script
# ------------------------------------------------
def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui  = app.userInterface

        # Grab the active design
        product = app.activeProduct
        design = adsk.fusion.Design.cast(product)
        rootComp = design.rootComponent

        # 1) Create the main assembly component.
        assemblyComp = createWeederAssembly(design, PARAMS)

        # 2) If desired, read or write parameter data from/to JSON
        # Example: export current param values to a JSON file
        # exportParamsToJSON(design, "C:/temp/weeder_params_out.json")

        ui.messageBox("Weeder assembly created successfully!")

    except:
        if ui:
            ui.messageBox("Failed:\n{}".format(traceback.format_exc()))


# ------------------------------------------------
# Create the overall tool assembly in the root
# ------------------------------------------------
def createWeederAssembly(design: adsk.fusion.Design, params: dict) -> adsk.fusion.Occurrence:
    rootComp = design.rootComponent
    assemblyOcc = rootComp.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    assemblyComp = assemblyOcc.component
    assemblyComp.name = "WeederAssembly"

    # 1) EMT conduit
    emtComp = createEMTConduit(design, params["EMT"])
    emtOcc = assemblyComp.occurrences.addExistingComponent(emtComp, adsk.core.Matrix3D.create())

    # 2) Gripper
    gripperComp = createGripperArm(design, params["Gripper"])
    gripperOcc = assemblyComp.occurrences.addExistingComponent(gripperComp, adsk.core.Matrix3D.create())
    # ... position or joint for the gripper as desired ...

    # 3) Filament spool
    spoolComp = createFilamentSpool(design, params["FilamentSpool"])
    spoolOcc = assemblyComp.occurrences.addExistingComponent(spoolComp, adsk.core.Matrix3D.create())

    # 4) Spool mount
    spoolMountComp = createSpoolMount(design,
                                      params["SpoolMount"],
                                      params["FilamentSpool"],
                                      params["EMT"])
    spoolMountOcc = assemblyComp.occurrences.addExistingComponent(spoolMountComp, adsk.core.Matrix3D.create())

    # TODO: Add joints or transforms to place spoolOcc & spoolMountOcc near bottom of EMT.
    # For instance, to place spoolMount at spoolOffsetFromBottom_mm:
    #   position it so mount's C-clamp center lines up with spoolMountParams['spoolOffsetFromBottom_mm']

    return assemblyOcc



# ------------------------------------------------
# Create the EMT conduit body
# ------------------------------------------------
def createEMTConduit(design: adsk.fusion.Design, emtParams: dict) -> adsk.fusion.Component:
    """Creates a simple cylindrical body representing the 1/2" EMT conduit."""
    # Make a new component
    rootComp = design.rootComponent
    newOcc = rootComp.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    comp = newOcc.component
    comp.name = "EMT_Conduit"

    od = emtParams["outerDiameter_mm"]
    id_ = emtParams["innerDiameter_mm"]
    length = emtParams["length_mm"]

    # Here, sketch a circle for OD, extrude to length.
    # Then optionally shell or revolve to get the tube shape.
    # Example pseudo-code:
    sketches = comp.sketches
    xyPlane = comp.xYConstructionPlane
    sketch = sketches.add(xyPlane)
    sketch.name = "EMT_CircleSketch" 

    # Draw OD circle
    circles = sketch.sketchCurves.sketchCircles
    centerPoint = adsk.core.Point3D.create(0, 0, 0)
    circleOD = circles.addByCenterRadius(centerPoint, od/2.0)

    # Extrude
    prof = sketch.profiles.item(0)
    extrudes = comp.features.extrudeFeatures
    extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
    distance = adsk.core.ValueInput.createByReal(length)
    extInput.setDistanceExtent(False, distance)
    ext = extrudes.add(extInput)
    ext.name = "EMT_Outer"

    # (Optional) Shell the extruded cylinder to get the inner diameter:
    shellFeats = comp.features.shellFeatures
    faces = adsk.core.ObjectCollection.create()
    faces.add(ext.endFaces[0])
    shellInput = shellFeats.createInput(faces, True)
    shellThickness = (od - id_)/2.0
    shellInput.insideThickness = adsk.core.ValueInput.createByReal(shellThickness)
    shellFeat = shellFeats.add(shellInput)
    shellFeat.name = "EMT_Shell"

    return comp


# ------------------------------------------------
# Create the Gripper Arm
# ------------------------------------------------
def createGripperArm(design: adsk.fusion.Design, gripParams: dict) -> adsk.fusion.Component:
    """
    Creates a simple parametric gripper with a 2-piece jaw,
    each jaw having a TPU finger pad.
    """
    newOcc = design.rootComponent.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    comp = newOcc.component
    comp.name = "GripperArm"

    # Access relevant parameters
    jawSpan = gripParams["jawSpan_mm"]
    armThk = gripParams["armThickness_mm"]
    armLen = gripParams["armLength_mm"]
    padThk = gripParams["tpuPadThickness_mm"]

    # Example geometry steps:
    # 1) Create a rectangular “arm bar”,
    # 2) Create the pivot hole for an M5 bolt,
    # 3) Create a jaw shape (like a pair of “fingers”),
    # 4) Possibly revolve or extrude a shape for the TPU pad.

    # PSEUDO-CODE for extrusions
    sketches = comp.sketches
    xyPlane = comp.xYConstructionPlane
    sketch = sketches.add(xyPlane)
    sketch.name = "Gripper_ArmBarSketch"
    lines = sketch.sketchCurves.sketchLines

    # Draw an outline for the arm
    p0 = adsk.core.Point3D.create(0, 0, 0)
    p1 = adsk.core.Point3D.create(armLen, 0, 0)
    p2 = adsk.core.Point3D.create(armLen, armThk, 0)
    p3 = adsk.core.Point3D.create(0, armThk, 0)
    l1 = lines.addByTwoPoints(p0, p1)
    l2 = lines.addByTwoPoints(p1, p2)
    l3 = lines.addByTwoPoints(p2, p3)
    l4 = lines.addByTwoPoints(p3, p0)

    prof = sketch.profiles.item(0)
    extrudes = comp.features.extrudeFeatures
    extInput = extrudes.createInput(prof, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
    distance = adsk.core.ValueInput.createByReal(1.0)  # some small thickness in the 3rd dimension
    extInput.setDistanceExtent(False, distance)
    armExtrude = extrudes.add(extInput)
    armExtrude.name = "Arm_Extrude"

    # TODO: Add geometry for jaws or revolve them. Then add the TPU pad
    # createTPUFingerPad(...)

    return comp


# ------------------------------------------------
# (Optional) Create the TPU Finger Pad
# ------------------------------------------------
def createTPUFingerPad(design: adsk.fusion.Design, padParams: dict) -> adsk.fusion.Component:
    """
    Example of a separate function for the finger pad.
    Could revolve or extrude a small ‘cup’ shape
    the user can easily replace.
    """
    # Implementation similar to createGripperArm
    pass


def createFilamentSpool(design: adsk.fusion.Design, spoolParams: dict) -> adsk.fusion.Component:
    """
    Creates a simplified geometry to represent a filament spool:
      - Outer diameter = spoolParams["outerDiameter_mm"]
      - Rim thickness = spoolParams["rimThickness_mm"]
      - Overall width = spoolParams["overallWidth_mm"]
      - Hub outer diameter = spoolParams["hubOuterDiameter_mm"]
      - Inner hole diameter = spoolParams["hubInnerDiameter_mm"]
    """
    newOcc = design.rootComponent.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    comp = newOcc.component
    comp.name = "FilamentSpool"

    od = spoolParams["outerDiameter_mm"]
    rimThk = spoolParams["rimThickness_mm"]
    width = spoolParams["overallWidth_mm"]
    hubOD = spoolParams["hubOuterDiameter_mm"]
    holeID = spoolParams["hubInnerDiameter_mm"]

    sketches = comp.sketches
    xyPlane = comp.xYConstructionPlane
    revolveFeatures = comp.features.revolveFeatures

    # 1) Create a sketch that profiles a cross-section of half the spool
    sketch = sketches.add(xyPlane)
    sketch.name = "SpoolDiscCrossSectionSketch"
    lines = sketch.sketchCurves.sketchLines

    # For simplicity, revolve around the X-axis or Y-axis. We'll revolve around the sketch's X-axis, for instance.
    # Let's define the spool center along the sketch origin.
    centerAxis = lines.addByTwoPoints(adsk.core.Point3D.create(0, -1000, 0),
                                      adsk.core.Point3D.create(0, 1000, 0))

    # We'll draw the cross-section from the axis outward:
    #  - One rim disc: 3 mm thick from R=od/2 to axis
    #  - Then a gap (the spool interior)
    #  - Then the other rim disc

    # For a basic revolve approach, we can revolve the outer shape (200 mm radius) minus the inside hole (55 mm).
    # But let's do just one side's disc, revolve 360, then extrude or revolve the hub.

    # Let's revolve the outer disc (one rim):
    # We'll do a line from (0,0) -> (od/2, 0) -> (od/2, rimThk) -> (0, rimThk), revolve that around the Y-axis.
    # Then do an offset to place the second disc. This is just one approach.

    # Pseudo-code cross-section for one disc:
    p0 = adsk.core.Point3D.create(0, 0, 0)
    p1 = adsk.core.Point3D.create(od/2, 0, 0)
    p2 = adsk.core.Point3D.create(od/2, rimThk, 0)
    p3 = adsk.core.Point3D.create(0, rimThk, 0)

    l1 = lines.addByTwoPoints(p0, p1)
    l2 = lines.addByTwoPoints(p1, p2)
    l3 = lines.addByTwoPoints(p2, p3)
    l4 = lines.addByTwoPoints(p3, p0)

    # Revolve the profile
    prof = sketch.profiles.item(sketch.profiles.count - 1) # last created
    revInput = revolveFeatures.createInput(prof, centerAxis, 
                                           adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
    angle = adsk.core.ValueInput.createByString("360 deg")
    revInput.setAngleExtent(False, angle)
    discRevolve = revolveFeatures.add(revInput)
    discRevolve.name = "SpoolDisc1"

    # Move or pattern the disc to create the second disc. For example:
    # We'll just copy the body and translate it by spool's overall width
    body1 = discRevolve.bodies.item(0)
    moveFeats = comp.features.moveFeatures
    bodiesToMove = adsk.core.ObjectCollection.create()
    bodiesToMove.add(body1)

    translateVector = adsk.core.Vector3D.create(0, width, 0)  # shift along Y

    transform = adsk.core.Matrix3D.create()
    transform.translation = translateVector  # ✅ Actually apply the translation
    moveInput = moveFeats.createInput(bodiesToMove, transform)

    moveInput.transform.translation = translateVector
    moveFeature = moveFeats.add(moveInput)
    moveFeature.name = "TranslateDisc1"

    # Alternatively, you could revolve a single cross-section that has both discs built in.
    # For clarity, we'll create the second disc by pattern or by revolve a second time.

    # 2) Create the hub (80 mm OD, 55 mm ID).
    # We'll revolve a thin cylinder bridging the distance between the two discs.
    # Then we’ll shell or revolve out the 55 mm hole.
    hubSketch = sketches.add(xyPlane)
    hubSketch.name = "SpoolHubCrossSectionSketch"
    hubLines = hubSketch.sketchCurves.sketchLines
    hubAxis = hubLines.addByTwoPoints(adsk.core.Point3D.create(0, -1000, 0),
                                      adsk.core.Point3D.create(0, 1000, 0))

    # cross-section for hub’s outer radius
    p0 = adsk.core.Point3D.create((hubOD/2), 0, 0)
    p1 = adsk.core.Point3D.create((hubOD/2), width, 0)
    l1 = hubLines.addByTwoPoints(p0, p1)
    # close with vertical axis line. We can rely on the revolve axis to close it.

    if hubSketch.profiles.count == 0:
        app = adsk.core.Application.get()
        ui = app.userInterface
        ui.messageBox("No profiles found in hubSketch, " + hubSketch.name)
        return comp  # or handle the error gracefully

    profiles = hubSketch.profiles
    if profiles.count > 0:
        hubProf = profiles.item(profiles.count - 1)
    else:
        raise ValueError("No profiles found in hubSketch," + hubSketch.name)

    hubRevInput = revolveFeatures.createInput(hubProf, hubAxis,
                                              adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
    hubRevInput.setAngleExtent(False, angle)
    hubRevolve = revolveFeatures.add(hubRevInput)
    hubRevolve.name = "SpoolHub"

    # Subtract the hole (55 mm) if you want actual geometry. Or revolve a second profile for the interior hole.
    # For brevity, we won't detail it here. You could do an extrude cut or revolve cut with a 55 mm radius circle.

    return comp


def createSpoolMount(design: adsk.fusion.Design,
                     mountParams: dict,
                     spoolParams: dict,
                     emtParams: dict) -> adsk.fusion.Component:
    """
    Creates a 'C-clamp' style spool mount that:
     - Wraps partway around spool's 55 mm hole, allowing spool to spin
     - Clamps onto 1/2" EMT (~17 mm OD)
     - Has 2 bolt holes for M5 (or whichever) to secure to the EMT
     - Optional grooves inside the clamp to reduce friction
    """

    newOcc = design.rootComponent.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    comp = newOcc.component
    comp.name = "SpoolMount"

    cClampWall = mountParams["cClampWallThickness_mm"]
    clearance = mountParams["cClampClearance_mm"]  # e.g. spool hole is 55 -> clamp OD ~ 54
    grooveDepth = mountParams["grooveDepth_mm"]
    grooveCount = mountParams["grooveCount"]

    spoolHoleID = spoolParams["hubInnerDiameter_mm"]  # 55 mm
    clampOuterDiameter = spoolHoleID - clearance      # e.g. ~54 mm
    clampRadius = clampOuterDiameter * 0.5

    # 1) Sketch the clamp's cross-section in XY plane
    sketches = comp.sketches
    xyPlane = comp.xYConstructionPlane
    sketch = sketches.add(xyPlane)
    sketch.name = "ClampRingCrossSectionSketch"

    # We'll revolve or extrude a "C" shape around Z or Y.
    # Let's do a revolve around the X-axis to form a ring, then
    # remove a section to create the "C" gap.

    revolveFeatures = comp.features.revolveFeatures

    # A ring cross-section: from radius=clampRadius to clampRadius + cClampWall
    # We'll revolve 360 then cut out a ~60-90 degree portion to form the open clamp.

    # Cross-section: a rectangle from (clampRadius, 0) to (clampRadius + cClampWall, 0) up some small height
    # because revolve wants a 2D shape. Actually, we can revolve a 2D line offset from center.
    # For brevity, we'll revolve a rectangular band.
    lines = sketch.sketchCurves.sketchLines

    # revolve axis along X
    revolveAxis = lines.addByTwoPoints(adsk.core.Point3D.create(0, -1000, 0),
                                       adsk.core.Point3D.create(0, 1000, 0))

    # Rectangle from r1 -> r2
    r1 = clampRadius
    r2 = clampRadius + cClampWall
    p0 = adsk.core.Point3D.create(r1, 0, 0)
    p1 = adsk.core.Point3D.create(r2, 0, 0)
    p2 = adsk.core.Point3D.create(r2, 1, 0)   # arbitrary 1 mm to create a profile
    p3 = adsk.core.Point3D.create(r1, 1, 0)

    l1 = lines.addByTwoPoints(p0, p1)
    l2 = lines.addByTwoPoints(p1, p2)
    l3 = lines.addByTwoPoints(p2, p3)
    l4 = lines.addByTwoPoints(p3, p0)

    prof = sketch.profiles.item(sketch.profiles.count - 1)
    revInput = revolveFeatures.createInput(prof, revolveAxis,
                                           adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
    angle = adsk.core.ValueInput.createByString("360 deg")
    revInput.setAngleExtent(False, angle)
    clampRevolve = revolveFeatures.add(revInput)
    clampRevolve.name = "ClampRing"

    # 2) Cut out a portion of the ring to form the "C" opening
    # Example: revolve cut or extrude cut over some angle (like 60 deg).
    # We'll do a quick revolve cut approach:
    cutSketch = sketches.add(xyPlane)
    cutSketch.name = "ClampRingCutSketch"
    cutLines = cutSketch.sketchCurves.sketchLines
    # We'll revolve a wedge that intersects the ring. Let’s define the wedge in 2D that covers e.g. 60 deg around revolveAxis.
    # For brevity, let's skip the details. A typical approach: revolve a rectangle from angleStart to angleEnd.

    # 3) Add friction-reducing grooves around the clamp interior
    # One approach: pattern a small extruded ring or revolve cut along the clamp's inner surface.
    # Pseudo-code only:
    # for i in range(grooveCount):
    #     create a revolve cut with a slight offset from the inner radius
    #     depth = grooveDepth
    #     angle spacing = 360 / grooveCount
    # ... etc.

    # 4) Add the portion that clamps onto the EMT
    #   - Typically a curved saddle for the 17 mm OD, plus 2 bolt holes to clamp around the pipe.
    # Pseudo-code:
    addEMTClamp(comp, emtParams["outerDiameter_mm"], mountParams)

    # Return the completed spool mount
    return comp


def addEMTClamp(spoolMountComp: adsk.fusion.Component,
                emtOuterDiameter: float,
                mountParams: dict):
    """
    Creates geometry for a bracket or partial wrap around 1/2" EMT.
    Drills two bolt holes for M5, etc.
    This will be integrated into the spool mount body so the
    user can tighten bolts to clamp on the conduit.
    """
    # Sketch a curved saddle that matches ~17 mm diameter, extrude around that shape,
    # then add two clearance holes for M5. Possibly add flanges for the bolts.

    # This is just skeleton code, so you can adapt:
    pass


# ------------------------------------------------
# Example utility: Export user parameters to JSON
# ------------------------------------------------
def exportParamsToJSON(design: adsk.fusion.Design, filePath: str):
    userParams = design.userParameters
    paramDict = {}
    for i in range(userParams.count):
        p = userParams.item(i)
        paramDict[p.name] = {
            "value": p.expression,
            "comment": p.comment
        }

    with open(filePath, 'w') as f:
        json.dump(paramDict, f, indent=2)

    print(f"Exported user parameters to {filePath}")

