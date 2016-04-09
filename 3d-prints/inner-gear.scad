include<gear.scad>

// Inner Gear
translate([30, 0, 0])
difference()
{
    // inner gear
    rotate([0,0,65]) color([0.75,0.75,1.00]) gear(mm_per_tooth,7,thickness,2);
    
    // hole for rotary encoder radius 7 and width 5 before flat part
    difference()
    {
        cylinder(h=10,r=3.2,center=true); 
        translate([3,0,0])cube([2,7,10], center=true);
    }
}
