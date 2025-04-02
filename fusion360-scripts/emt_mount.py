def create(rootComp):
    # Dimensions for 1/2" EMT (OD ≈ 16.7mm), allow clamping
    diameter = 17.5
    wall_thickness = 4
    mount_width = 40
    clamp_hole_diameter = 5

    # Create a new component
    comp = rootComp.occurrences.addNewComponent(adsk.core.Matrix3D.create()).component
    comp.name = "EMT_Mount"

    sketches = comp.sketches
    xyPlane = comp.xYConstructionPlane
    sketch = sketches.add(xyPlane)

    # Draw outer circle
    circles = sketch.sketchCurves.sketchCircles
    circles.addByCenterRadius(adsk.core.Point3D.create(0, 0, 0), diameter / 2)
    circles.addByCenterRadius(adsk.core.Point3D.create(0, 0, 0), (diameter - wall_thickness) / 2)

    prof = sketch.profiles.item(0)
    extrudes = comp.features.extrudeFeatures
    ext = extrudes.addSimple(prof, adsk.core.ValueInput.createByReal(mount_width), adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

    # Add holes for clamping
    hole_sketch = sketches.add(comp.xYConstructionPlane)
    hole_sketch.sketchPoints.add(adsk.core.Point3D.create(diameter / 2 + 10, 0, 0))
    # [Add extruded hole features later]

    return comp
