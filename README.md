# Project: Pet Sapling Savior, or, Pet Weed Euthanizer?

## Description
An Arduino‐powered, Mostly 3D‐printed weeder that grips unwanted plants, gently uproots them, and deposits sapling-worthy seedlings into your chosen grow tray—combining garden cleanup, and with mini reforestation.

### WHY?
- I hate weeding, and avoid chemicals, plus I'd like to relocate otherwise unwanted saplings to areas needing reforesting, or, yards of people that like/deserve weeds 😉.  
- I want AI machines to do my bidding, before they make me do theirs.
 
### WHAT's SPECIAL?
- Fast cordless yard fun way to relocate pet saplings, or, euthanize pet weeds.  
- Design includes many components that can be repurposed for other DIY tools.
- Today's [Best selling Weeders](https://www.amazon.com/gp/bestsellers/lawn-garden/553934) are either designed for large deep rooted weeds.  They don't help with small weeds.  So today, people need to bend down, or kneel down, to grip and pull out small weeds.  
- Cutting/trimming weeds is fast for a day...  But, requires multiple cuts over long periods to eventually exhaust weeds.
- FluidNC controller boards doesn't require code, just upload config via WebUI, and go relocating/hunting...

## Functions
- Grip, pull and discard, or, collect for relocation.
- Normal grip and pull.
- Turbo grip and pull, mode for rapid munching, and suborbital sapling/weed launching. 

## 2. Hardware & Software
- [ ] **Hardware List**: 
  - See [BOM.xlsx](https://github.com/aaronse/weed-puller/BOM.xlsx) for full common hardware list, or markdown format [BOM Hardware](https://github.com/aaronse/weed-puller/bom-hardware.md).
    - BOM contains full list of all parts I personally tried for various iterations/version.  Recommend picking 2 steppers and drivers you have available already, or, your budget and interest.  
      - Recommend 2x Nema 17 84oz with 2x TMC2209 for max pulling fun :-)
  - [ ] Provided info to build either (A) Arduino, or (B) ESP32 based build :
    - [ ] Option (A), Arduino Controller:
      - [ ] board, stepper motor(s), gears, sensors, switches, battery, etc.
    - [ ] Option (B), V1 Engineering JackPot Controller (ESP32, FluidNC): 
      - [ ] board, stepper motor(s), gears, sensors, switches, battery, etc.
- [x] **Software List**: CAD/design tools and libraries...
  - VS Code, Arduino IDE, Platform IO, FluidNC/ESP3D, Fusion 360, OpenSCAD.

### 3. Wiring Diagram
- [ ] Draw clear schematic (PNG/PDF).
- [ ] Annotate each connection with a brief caption.

### 4. Code
- [ ] Fully commented Arduino sketch.
- [ ] Summary of code behavior.
- [x] Link to full repository (e.g. GitHub URL).
  - [https://github.com/aaronse/weed-puller](https://github.com/aaronse/weed-puller)

### 5. 3D Printing Instructions
_For each STL file:_
- [ ] **Material** (e.g. PETG)
- [ ] **Layer height** (e.g. 0.2 mm)
- [ ] **Infill** (e.g. 20 %)
- [ ] **Supports** (Yes/No + location)


### 6. Assembly Instructions
- [ ] Step-by-step bullets (e.g. “1. Insert stepper into jaw housing; 2. Route wires through handle; …”).
- [ ] Alignment or torque tips (e.g. jaw mesh clearance).

### 7. Demo & Usage Instructions
- [ ] Power-on and mode selection (Pull vs. Release).
- [ ] Operating tips (e.g. hold at 30° angle, watch for stall-flash).
- [ ] Safety note (keep fingers clear of gears).

### 8. Additional Notes & Tips
- [ ] Troubleshooting pointers (e.g. check wiring if it stalls).
- [ ] Optional enhancements (e.g. add LCD status display).

### 9. Photos & Videos
- [ ] Finished product: multiple angles.
- [ ] Wiring close-ups.
- [ ] In-action demo GIF or short video (pull + relocate cycle).

### 10. Final Packaging & Submission
- [ ] Populate the `Printable_Arduino_Project_Template.pdf` with your content.
- [ ] Upload all STLs, code, diagrams, and the filled template to Printables.com.
- [ ] Ensure safety—no hazardous features.
- [ ] Submit before **Apr 27, 2025** (use code `PRINTABLES25` if you need parts).

### 11. (Optional) Clout Boosters
- [ ] Embed your YouTube demo link in the project description.
- [ ] Share teaser clips on social media with `#PrintablesContest`.






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


## 📄 License

⚠️ This project is for **personal, non-commercial use only**.

🚫 You may not sell, manufacture, or commercially distribute this project or any derivative works.

😏 *Wanna sell it anyway?*  
Get in touch — am open to licensing deals to help pay for my kids' college fees...
Contact us at: https://github.com/aaronse/weed-puller/issues

This project is licensed under the [Creative Commons BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/) license.

You are free to use and remix for **personal, non-commercial use**.  
**Commercial use or resale of any parts, models, or derivatives is strictly prohibited.**
