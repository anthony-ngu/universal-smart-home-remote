include<gear.scad>

// 2.5 & 14

// Gear Portion
union()
{   
    // top ring
    difference()
    {
        translate([0,0,10])cylinder(h = 10, r = 50, center = true);
        translate([0,0,10])cylinder(h = 10, r = 48, center = true);
    }
    
    // bottom ring
    difference()
    {
        translate([0,0,-17])cylinder(h = 24, r = 50, center = true);
        translate([0,0,-17])cylinder(h = 24, r = 48, center = true);
    }
    
    // the middle gear portion
    difference()
    {
        cylinder(h = 10, r = 50, center = true);
        rotate([0,0,30]) color([0.75,1.00,0.75]) gear(mm_per_tooth,30,thickness,hole,0,108);
    }
}