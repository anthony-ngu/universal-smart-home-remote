
// Top Layer
union(){
    // The top lid
    difference()
    {
        translate([0,0,22])cylinder(h = 5, r = 48, center = true);
        translate([0,0,20])cylinder(h = 3, r = 45, center = true);
    }
    
    difference()
    {
            translate([0,0,16])cylinder(h = 8, r = 47, center = true);
            translate([0,0,16])cylinder(h = 8, r = 45, center = true);
    }
    
    // The rod for the button on the inner gear
    translate([30, 0, 18])
    {
        cylinder(h=10,r=3,center=true);
    }
    
    // The rod for the spring location
    translate([-30,0,18])
    {
        cylinder(h=7,r=3,center=true);
        translate([0,0,-1]) difference()
        {
            cylinder(h=15,r=5.5,center=true);
            cylinder(h=15,r=4.5,center=true);
        }
    }
}