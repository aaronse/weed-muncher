
- Trigger Design
  - https://www.youtube.com/watch?v=H3J8FiLNEtc
  1:07
  - Airwolf trigger, turbo, rockets...
    - 0.35 https://www.youtube.com/watch?v=PigDNRQyNwE
https://installer.fluidnc.com/

## Firmware
- FluidNC
  - http://wiki.fluidnc.com/en/config/control
  - Macros http://wiki.fluidnc.com/en/config/macros
  - Configuring Pins http://wiki.fluidnc.com/en/config/config_IO
  - ESP32 Dev Kit Pins http://wiki.fluidnc.com/en/hardware/esp32_pin_reference
  - IPC via WebSockets/TCP http://wiki.fluidnc.com/en/support/interface/websockets
  - 

## WHAT / WHY...
Semi-automated Weed Puller intended to be used for small broad leaf weed situations where GramPa's Weeder (and similar) are unable to grip.

For example, I have a ton of unwanted Maple tree weedlings.

## TODO/TASKS
- CAD Design edits
  - Prevent weeds sneaking behind, add funnel/guide?

  - Grinder Assembly
    - Grinder Core
      - Add lip to stop tire pulling towards Flex Mount plate
    - Grinder Tire
      - Reduce rotation angle so 0.4mm layer height slicer doesn't add "top" line segments. 
      - Prevent slip off: More bumps/recesses, or make existing bigger?
      - Modify design so Slicer 0.72mm wall width results in _inner hub wall_ and _tooth wall_ touching/bonding.

## HISTORY
- 1st Tire Print 
  - (3hr 12mins, 15% retractions) Slicer config: 2 walls, alternative wall, infill 20%, 0.32mm layer height
  - Stiffer than expected
- 2nd Grinder Tire print ()
  - (1hr 51mins, 11%) Slice config: 2 walls, 0.66mm, line width, NO alternative wall, 0% infill, 0.36mm layer height
    - ⚠️🤷‍♂️ Counter intuitively, SAVED print time using 0.32mm layer height instead of 0.4mm.  Reason is steep overhang angle of the gear teeth results in more wall material being added, when laying thick layers like 0.4mm.  Added task to reduce rotation angle of sweep, so thicker faster single walls are possible, 
  - Goals flexible print that still grips Grinder Core

## DESIGN
- Grinder Flex Mount
  - Design uses _stepped recess bore holes_, looked at [various bottom recess options](./docs/cads-flat-bottom-recess.htm)
   - To support grabbing weeds of varying thickness, the design uses linear spring compliant mechanism for resistive flex.  Goal is 30N or more resistive force, 3.5mm limited range of motion.
     - Considered [pros/cons of various materials for compliant mechanisms](./docs/compliant-mechanism-materials.htm)
     - Considered [various Open Source FEM software](./docs/fem-software-options.htm)
  - Intentionally spaced mount holes 1" OC for optional pegboard mounting.

- Grinder Wheel Treads
  - Design has conical features with tiller
    - Fusion 360 Coils swept along spiral tapered path 
    - https://www.youtube.com/watch?v=8WW2_HtuV9A
  - Morley Kert, Fusion 360 "Create Form" to make twisted scaled structures Twist
    - https://www.youtube.com/watch?v=FTLb_tus4F4



- Pole to Grinder mounts
  - History/context:
    - Was concerned about 3D print being strong enough.  Designing and printing anyway to confirm/correct my understanding/theory with data...
    - Considered 3D printed like... 
      ![alt text](img/cad-meh-bracket-mount.png)
    - Considered common easy to source brackets.  Ideally 1" wide with centered holes.
      - Best Selling Corner Braces https://www.homedepot.com/b/Building-Materials-Building-Hardware-Corner-Braces/N-5yc1vZc8hr 
      - https://www.homedepot.com/p/Everbilt-1-in-Steel-Zinc-Plated-Corner-Brace-4-Pack-24590/327599496

## PARTS

- PegBoard (alternative to CNC/Printed plates).
  - Popular 1" OC, 1/4" holes, Pegboard at HomeDepot:
    - Best Seller... Hardboard, painted White Acrylic  [48 in. H x 24 in. W White Pegboard](https://www.homedepot.com/p/Handprint-48-in-H-x-24-in-W-White-Pegboard-109099/202093803)
    - [16 in. H x 16 in. W Plastic Pegboard in Black (50 lbs.)](https://www.homedepot.com/p/Everbilt-16-in-H-x-16-in-W-Plastic-Pegboard-in-Black-50-lbs-814450/317815422)
    - Some popular/accessible options...
      ![image|845x892,50%](./img/buy-pegboard.png)
