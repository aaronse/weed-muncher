def create(parentComp):
    sketches = parentComp.sketches
    plane = sketches[0].referencePlane
    sketch = sketches.add(plane)

    width = 12
    height = 20
    thickness = 2

    sketch.sketchCurves.sketchLines.addTwoPointRectangle(
        adsk.core.Point3D.create(5, 5, 0),
        adsk.core.Point3D.create(5 + width, 5 + height, 0)
    )
    prof = sketch.profiles.item(0)
    parentComp.features.extrudeFeatures.addSimple(prof, adsk.core.ValueInput.createByReal(thickness), adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
