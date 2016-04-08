
module semi_circle(
    height=10,
    outer_radius=38,
    inner_radius=20,
    offset=[35,0,0]
)
{
    difference()
    {
        cylinder(h = height, r = outer_radius, center = true);
        translate(offset)cylinder(h = height, r = inner_radius, center = true);
    }
};

// lower lid
union()
{
    // The inner barrier
    translate([0,0,0])
    difference()
    {
        semi_circle(
            height=30,
            outer_radius=38,
            inner_radius=20,
            offset=[35,0,0]
        );
        semi_circle(
            height=30,
            outer_radius=35,
            inner_radius=24,
            offset=[35,0,0]
        );
        translate([15,0,-10]) cube(size=[10,40,50],center=true);
        translate([0,-33,-10]) cube(size=[40,10,50],center=true);
    }

    // The Base
    translate([0,0,-22])
    difference()
    {
        cylinder(h = 10, r = 50, center = true);
        translate([0,0,2.5])cube([35,69,5],center=true);
    }
    difference()
    {
        translate([0,0,-16])cylinder(h = 8, r = 47, center = true);
        translate([0,0,-16])cylinder(h = 8, r = 45, center = true);
    }
    
    // The Rod
    translate([-30,0,0])union()
    {
        translate([0,0,6])cylinder(h=13,r=3,center=true);
        translate([0,0,-10])cylinder(h=20,r=5,center=true);
    }
}