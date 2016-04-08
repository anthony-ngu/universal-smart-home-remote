
// Top Layer
union(){
    // The top lid
    translate([0,0,22])cylinder(h = 5, r = 50, center = true);
    difference()
    {
            translate([0,0,16])cylinder(h = 8, r = 47, center = true);
            translate([0,0,16])cylinder(h = 8, r = 45, center = true);
    }
    
    // The rod for the button on the inner gear
    translate([30, 0, 17])
    {
        cylinder(h=10,r=3,center=true);
    }
    
    // The rod for the spring location
    translate([-30,0,17])
    {
        cylinder(h=10,r=3,center=true);
    }
}