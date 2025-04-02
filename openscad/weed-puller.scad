//
// WeedGripperTool v0.1
// Single-file parametric OpenSCAD example for a weed-pulling tool.
// By [Your Name or Organization], [Date]
//
// NOTES:
//  - This script is a conceptual starting point with placeholder geometry.
//  - You should refine geometry for strength, print orientation, CNC details, etc.
//  - Modules are designed to be easily re-used or rearranged as you iterate.
//
// LICENSE: Whatever license you prefer (e.g., MIT, CC-BY, GPL, etc.)
//

//----------------------------------------
// Top-Level Parameters & Constants
//----------------------------------------

// Unit conversion for convenience
function inToMM(inches) = inches * 25.4; 

// 1/2" EMT approximate dimensions
emtOD = 18;          // Outer diameter in mm (nominal ~17.9mm)
emtID = 16;          // Inner diameter in mm (nominal ~15.8mm)

// Overall tool length (EMT conduit)
toolLength = inToMM(48);  // 48" ~ 1219mm

// Handle/Lever geometry
handleHeightFromBottom = inToMM(40);  // Lever/trigger ~40" from bottom
handleOffset = 30;                   // How far handle protrudes from pipe
handleThickness = 5;                 // For extrusions, placeholders

// Jaw/gripper geometry
jawMaxSpan = 50;     // ~2" in mm, max open span of jaws
jawDepth = 30;       // Some arbitrary depth for 3D printed jaws
jawThickness = 5;    // Basic thickness for each jaw half

// Wheel / pivot geometry
useWheels = true;
wheelDiameter = 90;   // e.g., a reused 3D filament spool or small wheel
wheelThickness = 25;  // approximate thickness for spool-type wheel
wheelMountThickness = 5;

// Belt/Pulley placeholders
beltWidth = 10;   // GT2 belt 10 mm wide
pulleyOD = 18;    // Example GT2 pulley OD, placeholders
idlerOD = 8;      // Example GT2 idler OD, placeholders

// Screw & hardware parameters
boltHoleDiameter = 5.5;    // For M5 bolts
nutTrapDepth = 6;          // Param for nylock or standard nut captures
plateThicknessAcrylic = 6; // e.g., 1/4" ~ 6.35mm for acrylic
plateThicknessPlywood = 12; // e.g., 1/2" ~ 12.7mm for plywood

// Pro version includes optional AI hardware mount
ProVersion = false;  // If true, adds Pi Camera, servo/stepper placeholders, etc.

// Debug / assembly
showExploded = false; 
explodeOffset = 50;  // How far to shift components for clarity

//----------------------------------------
// Modules
//----------------------------------------

//
// 1) EMT Conduit Placeholder
//
// This is just a visual placeholder for the tube you’ll supply
// Users will purchase their own 1/2" EMT conduit (48" long).
//
module showEMT() {
    color("silver") 
    cylinder(h=toolLength, r=emtOD/2, center=false, $fn=64);
}

//
// 2) EMT Mount (for wheel pivot, for handle, etc.)
// Param: height -> the position along the EMT to mount
//        thickness -> extrude thickness
//        name -> string for identification
//
module emtMount(height, thickness=5, name="emtMount") {
    // Basic ring clamp that attaches around the EMT
    translate([0,0,height])
    difference() {
        cylinder(h=thickness, r=emtOD/2+3, center=false, $fn=64); // Outer
        cylinder(h=thickness+0.1, r=emtOD/2, center=false, $fn=64); // Subtract ID
    }
}

//
// 3) Lever/Trigger Handle
// Very rough "rifle style" handle shape for demonstration
// You can refine the shape to be more comfortable and aesthetic.
//
module handle() {
    // A simplistic handle: a vertical rectangular extrusion offset from the pipe.
    translate([emtOD/2 + handleOffset,0,handleHeightFromBottom]) {
        color("red")
        cube([handleThickness, 30, 100], center=false);
    }
}

//
// 4) Gripper Jaws
// Two opposing jaws that pinch around a center axis. 
// We'll do a simplistic shape here. Each jaw is a box with a curved tip. 
// You can adapt more specialized geometry (fingerlike shape, etc.).
//
module gripperJaws() {
    // "Left" jaw
    translate([-jawMaxSpan/2,0,0]) {
        color("green")
        union() {
            // Main block
            cube([jawMaxSpan/2, jawThickness, jawDepth], center=false);
            // Curved tip for comedic "finger" shape
            translate([jawMaxSpan/2, jawThickness/2, jawDepth/2]) {
                cylinder(h=jawThickness, r=jawThickness, $fn=32, center=true);
            }
        }
    }
    // "Right" jaw
    translate([0,0,0]) {
        color("green")
        union() {
            cube([jawMaxSpan/2, jawThickness, jawDepth], center=false);
            translate([0, jawThickness/2, jawDepth/2]) {
                cylinder(h=jawThickness, r=jawThickness, $fn=32, center=true);
            }
        }
    }
}

//
// 5) TPU Finger Pads
// Example insert that fits over the jaws or has a friction fit
// In real design, you'd ensure geometry for tension, alignment, etc.
//
module tpuFingerPad() {
    color("yellow")
    // Just a small piece that can be glued or friction-fit on top of each jaw tip.
    // We'll define a single pad; you'd instance it in place for each jaw if needed.
    translate([0,0,jawDepth]) {
        cube([jawMaxSpan/2, jawThickness, 3], center=false);
    }
}

//
// 6) Pivot/Wheel Module
// This could be a 3D-printed or CNC cut bracket that mounts a wheel or spool
// near the bottom of the tool so it can roll/pivot, making it easier to lever weeds.
// 
module pivotWheelMount() {
    // We'll place the wheel near the bottom, with the bracket around the EMT
    // For demonstration, we create a ring clamp plus an axle for the wheel
    color("blue")
    union() {
        // The clamp around the EMT
        emtMount(0, wheelMountThickness, "wheelClamp");

        // Axle for wheel
        translate([emtOD/2 + wheelDiameter/2 + 2, 0, wheelMountThickness/2]) {
            cylinder(h=10, r=3, center=true, $fn=32);
        }
    }
}

//
// 7) Wheel/Omni-wheel or spool
// The actual rolling element. Could be an omni-wheel or spool from 3D filament.
//
module pivotWheel() {
    color("orange")
    translate([emtOD/2 + wheelDiameter/2 + 2, 0, wheelMountThickness/2]) {
        cylinder(h=wheelThickness, r=wheelDiameter/2, center=true, $fn=64);
    }
}

//
// 8) Belt/Pulley/Idler placeholders
// The actual path and tension mechanism is up to you—this is just a demonstration.
//
module beltSystem() {
    color("purple")
    // Show a pulley near handle
    translate([emtOD/2 + handleOffset, 0, handleHeightFromBottom + 50]) {
        cylinder(h=beltWidth, r=pulleyOD/2, center=true, $fn=32);
    }
    
    // Show an idler near jaws (you'd refine exact positioning)
    translate([-(emtOD/2 + 10), 0, 50]) {
        cylinder(h=beltWidth, r=idlerOD/2, center=true, $fn=32);
    }
    
    // Just a placeholder belt as a rectangular block
    // In real design, you'd extrude a path or show a more accurate belt shape.
    translate([-emtOD, -beltWidth/2, 50]) 
    cube([emtOD*2, beltWidth, handleHeightFromBottom], center=false);
}

//
// 9) ProVersion Electronics
// If ProVersion is true, we add mounts for a Pi Zero, camera, servo/stepper, etc.
//
module proElectronicsMount() {
    if (ProVersion) {
        color("cyan")
        translate([emtOD/2 + handleOffset + 50, 0, handleHeightFromBottom]) {
            // Pi Zero 2W placeholder: ~65mm x 30mm board
            cube([65,30,5], center=false);
        }
        // Camera mount, servo/stepper placeholders, etc. 
        translate([emtOD/2 + handleOffset + 60, 0, handleHeightFromBottom + 60]) {
            cube([30,30,30], center=false);
        }
    }
}

//
// 10) Complete Bottom Gripper Assembly
// For clarity, we combine jaws, finger pads, belt system, etc. 
// and position them at the bottom of the EMT.
//
module bottomAssembly() {
    // We'll place everything near the bottom. 
    // In reality, you'd want a pivot or scissor mechanism to open/close jaws.
    
    // Jaws
    translate([0, -jawThickness/2, 0]) rotate([0,0,0]) {
        gripperJaws();
    }
    // Finger pads on each jaw
    // Left jaw pad
    translate([-jawMaxSpan/2, -jawThickness/2, 0]) {
        tpuFingerPad();
    }
    // Right jaw pad
    translate([0, -jawThickness/2, 0]) {
        tpuFingerPad();
    }
    
    // Example belt system that might connect up the conduit
    beltSystem();
}

//
// 11) Main Assembly
//
module mainAssembly() {
    
    // Show EMT Tube
    showEMT();
    
    // Wheel pivot near bottom
    if (useWheels) {
        pivotWheelMount();
        pivotWheel();
    }
    
    // The handle/trigger near the top
    handle();
    
    // The bottom clamp/gripper system
    bottomAssembly();
    
    // Optional Pro electronics
    proElectronicsMount();
}

//
// 12) Exploded view
// If showExploded is true, separate each sub-assembly for demonstration
//
module explodedAssembly() {
    if (showExploded) {
        // We shift each sub component in X or Y for demonstration
        translate([0,0,0]) showEMT();
        translate([explodeOffset,0,0]) pivotWheelMount();
        translate([explodeOffset*2,0,0]) pivotWheel();
        translate([0,explodeOffset,0]) handle();
        translate([explodeOffset,explodeOffset,0]) bottomAssembly();
        translate([explodeOffset*2,explodeOffset,0]) proElectronicsMount();
    } else {
        mainAssembly();
    }
}

//----------------------------------------
// Final call: render the entire design
//----------------------------------------
explodedAssembly();
