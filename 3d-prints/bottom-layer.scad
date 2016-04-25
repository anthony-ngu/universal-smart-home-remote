include <inner-barrier.scad>

// lower lid
union()
{
    // The Base
    translate([0,0,-21])
    difference()
    {
        cylinder(h = 8, r = 50, center = true);
        translate([0,0,2])cube([35,69,10],center=true); //wireless charger
        translate([0,0,3])cube([54,63,5.8],center=true);// li-ion battery 2000mah
        
        // The connector rods
        translate([-30,0,2])cylinder(h=10,r=3,center=true);
        translate([28.7,21.5,2])cylinder(h=10,r=2,center=true);
        translate([28.7,-21.5,2])cylinder(h=10,r=2,center=true);
    }
    
    // Inner Rim for combining pieces
    difference()
    {
        translate([0,0,-16])cylinder(h = 8, r = 47, center = true);
        translate([0,0,-16])cylinder(h = 8, r = 45, center = true);
    }
    
    //inner_barrier();
    
}