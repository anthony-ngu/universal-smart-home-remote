
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


// The inner barrier
module inner_barrier()
{
    union(){
        translate([0,0,-2.5])
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
            translate([15,0,-10]) cube(size=[10,40,10],center=true);
            //translate([0,-33,-10]) cube(size=[40,10,50],center=true);
            translate([0,0,-16])cube([35,69,5],center=true);
        }
        
        // The Rod
        translate([-30,0,0])union()
        {
            translate([0,0,3])cylinder(h=13,r=3,center=true);
            translate([-2,0,-10])cube([10,10,15],center=true);
        }
        
        // The connector rods
        translate([-30,0,-20])cylinder(h=10,r=3,center=true);
        translate([28.7,21.5,-20])cylinder(h=10,r=2,center=true);
        translate([28.7,-21.5,-20])cylinder(h=10,r=2,center=true);
    }
};

inner_barrier();