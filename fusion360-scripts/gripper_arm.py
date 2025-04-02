def create(rootComp, is_left=True):
    length = 75
    width = 15
    thickness = 8
    pivot_offset = 10

    name = "Gripper_Arm_Left" if is_left else "Gripper_Arm_Right"
    xOffset = -25 if is_left else 25

    comp = rootComp.occurrences.addNewComponent(adsk.core.Matrix3D.create()).component
    comp.name = name

    sketch = comp.sketches.add(comp.xYConstructionPlane)
    rect = sketch.sketchCurves.sketchLines.addTwoPointRectangle(
        adsk.core.Point3D.create(xOffset, 0, 0),
        adsk.core.Point3D.create(xOffset + length, width, 0)
    )
    prof = sketch.profiles.item(0)
    ext = comp.features.extrudeFeatures.addSimple(prof, adsk.core.ValueInput.createByReal(thickness), adsk.fusion.FeatureOperations.NewBodyFeatureOperation)

    return comp
