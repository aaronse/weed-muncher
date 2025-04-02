// ==== PARAMETERS ====
// Filament spool
spool_outer_diameter = 200;
spool_outer_thickness = 3;
spool_hub_diameter = 80;
spool_hub_thickness = 61;
spool_core_diameter = 55;

// EMT conduit (1/2" nominal, 16.7mm OD)
emt_outer_diameter = 16.7;
emt_wall_thickness = 1.1;  // Typical EMT
emt_length = 100;          // Sample length for modeling

// ==== MODULES ====
module filament_spool() {
    difference() {
        union() {
            // Outer side disks
            translate([0, 0, 0])
                cylinder(h = spool_outer_thickness, d = spool_outer_diameter, $fn=120);
            translate([0, 0, spool_hub_thickness])
                cylinder(h = spool_outer_thickness, d = spool_outer_diameter, $fn=120);

            // Central hub
            translate([0, 0, spool_outer_thickness])
                cylinder(h = spool_hub_thickness, d = spool_hub_diameter, $fn=120);
        }
        // Hollow core (center hole)
        translate([0, 0, -1])
            cylinder(h = spool_outer_thickness * 2 + spool_hub_thickness + 2, d = spool_core_diameter, $fn=100);
    }
}

module emt_conduit() {
    difference() {
        cylinder(h = emt_length, d = emt_outer_diameter, $fn=64);
        // Hollow center
        cylinder(h = emt_length + 2, d = emt_outer_diameter - 2 * emt_wall_thickness, $fn=64);
    }
}

// ==== RENDER ====
translate([-120, 0, 0])
    filament_spool();

translate([80, 0, 0])
    rotate([90, 0, 0])
    emt_conduit();
