import adsk.core, adsk.fusion, traceback

def run(context):
    app = adsk.core.Application.get()
    ui  = app.userInterface
    design = app.activeProduct
    rootComp = design.rootComponent

    # === PARAMETERS ===
    spool_params = {
        'outer_diameter': 200,    # mm
        'outer_thickness': 3,     # mm, thickness of each flange
        'hub_diameter': 80,       # mm, overall hub outer diameter
        'hub_thickness': 61,      # mm, hub axial thickness
        'core_diameter': 55       # mm, hollow center in the hub
    }

    emt_params = {
        'outer_diameter': 16.7,   # mm
        'wall_thickness': 1.1,    # mm
        'length': 100             # mm (for modeling)
    }

    # Helper: Create an offset construction plane from a given plane
    def offset_plane(base_plane, offset_mm):
        planes = rootComp.constructionPlanes
        planeInput = planes.createInput()
        offsetValue = adsk.core.ValueInput.createByReal(offset_mm)
        planeInput.setByOffset(base_plane, offsetValue)
        return planes.add(planeInput)

    # === CREATE FILAMENT SPOOL FUNCTION ===
    def create_filament_spool(comp, params, offset=(0,0,0)):
        sketches = comp.sketches
        extrudes = comp.features.extrudeFeatures
        xyPlane = comp.xYConstructionPlane

        # --------- Bottom Flange ---------
        sketch_bottom = sketches.add(xyPlane)
        centerPoint = adsk.core.Point3D.create(*offset)
        sketch_bottom.sketchCurves.sketchCircles.addByCenterRadius(centerPoint, params['outer_diameter']/2)
        prof_bottom = sketch_bottom.profiles.item(0)
        extInput_bottom = extrudes.createInput(prof_bottom, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extInput_bottom.setDistanceExtent(False, adsk.core.ValueInput.createByReal(params['outer_thickness']))
        bottomExtrude = extrudes.add(extInput_bottom)
        bottomBody = bottomExtrude.bodies.item(0)
        
        # --------- Central Hub ---------
        # Create hub as a full solid cylinder first.
        hub_plane = offset_plane(xyPlane, params['outer_thickness'])
        sketch_hub = sketches.add(hub_plane)
        sketch_hub.sketchCurves.sketchCircles.addByCenterRadius(centerPoint, params['hub_diameter']/2)
        prof_hub = sketch_hub.profiles.item(0)
        extInput_hub = extrudes.createInput(prof_hub, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extInput_hub.setDistanceExtent(False, adsk.core.ValueInput.createByReal(params['hub_thickness']))
        hubExtrude = extrudes.add(extInput_hub)
        hubBody = hubExtrude.bodies.item(0)
        
        # Now cut the inner core from the hub.
        # Create a sketch on the bottom face of the hub (which lies on hub_plane).
        hubBottomFace = None
        for face in hubBody.faces:
            # Find a planar face whose center's Z is close to the hub_plane offset.
            if face.geometry.classType() == adsk.core.Plane.classType():
                bb = face.boundingBox
                if abs(bb.minPoint.z - hub_plane.origin.z) < 0.01:
                    hubBottomFace = face
                    break

        if hubBottomFace:
            sketch_core = sketches.add(hubBottomFace)
            sketch_core.sketchCurves.sketchCircles.addByCenterRadius(centerPoint, params['core_diameter']/2)
            prof_core = sketch_core.profiles.item(0)
            extInput_core = extrudes.createInput(prof_core, adsk.fusion.FeatureOperations.CutFeatureOperation)
            extInput_core.setDistanceExtent(False, adsk.core.ValueInput.createByReal(params['hub_thickness']))
            extrudes.add(extInput_core)
        else:
            ui.messageBox("Could not find the hub bottom face for cutting the core.")

        # --------- Top Flange ---------
        # Create top flange on an offset plane from the XY plane at (outer_thickness + hub_thickness).
        top_plane = offset_plane(xyPlane, params['outer_thickness'] + params['hub_thickness'])
        sketch_top = sketches.add(top_plane)
        sketch_top.sketchCurves.sketchCircles.addByCenterRadius(centerPoint, params['outer_diameter']/2)
        prof_top = sketch_top.profiles.item(0)
        extInput_top = extrudes.createInput(prof_top, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extInput_top.setDistanceExtent(False, adsk.core.ValueInput.createByReal(params['outer_thickness']))
        extrudes.add(extInput_top)

    # === CREATE EMT FUNCTION ===
    def create_emt(comp, params, offset=(0,0,0)):
        sketches = comp.sketches
        extrudes = comp.features.extrudeFeatures
        yzPlane = comp.yZConstructionPlane

        sketch_emt = sketches.add(yzPlane)
        centerPoint = adsk.core.Point3D.create(*offset)
        sketch_emt.sketchCurves.sketchCircles.addByCenterRadius(centerPoint, params['outer_diameter']/2)
        sketch_emt.sketchCurves.sketchCircles.addByCenterRadius(centerPoint, params['outer_diameter']/2 - params['wall_thickness'])
        # Fusion 360 should recognize an annular profile
        prof_emt = sketch_emt.profiles.item(0)
        extInput_emt = extrudes.createInput(prof_emt, adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        extInput_emt.setDistanceExtent(False, adsk.core.ValueInput.createByReal(params['length']))
        extrudes.add(extInput_emt)

    # === EXECUTE FUNCTIONS ===
    try:
        create_filament_spool(rootComp, spool_params, offset=(-120, 0, 0))
        create_emt(rootComp, emt_params, offset=(80, 0, 0))
        ui.messageBox('Spool and EMT created successfully!')
    except:
        ui.messageBox('Failed:\n{}'.format(traceback.format_exc()))
