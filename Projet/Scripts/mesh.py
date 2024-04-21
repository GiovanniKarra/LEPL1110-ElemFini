import gmsh
from numpy import sin, cos, pi
from sys import argv


def create_ring(radius, size):
	center = gmsh.model.geo.add_point(0, 0, 0, size)
	start = gmsh.model.geo.add_point(radius, 0, 0, size)
	end = gmsh.model.geo.add_point(-radius, 0, 0, size)

	start2 = gmsh.model.geo.add_point(radius-0.1, 0, 0, size)
	end2 = gmsh.model.geo.add_point(-radius+0.1, 0, 0, size)

	arc1 = gmsh.model.geo.add_circle_arc(start, center, end)
	arc2 = gmsh.model.geo.add_circle_arc(end, center, start)

	arc3 = gmsh.model.geo.add_circle_arc(start2, center, end2)
	arc4 = gmsh.model.geo.add_circle_arc(end2, center, start2)

	face = gmsh.model.geo.add_curve_loop([arc1, arc2, arc3, arc4])
	
	return face


def ring_connection(from_radius, to_radius, angle, size):
	point1 = gmsh.model.geo.add_point(
		cos(angle+0.2)*from_radius, sin(angle+0.2)*from_radius, 0, size)
	point2 = gmsh.model.geo.add_point(
		cos(angle-0.2)*from_radius, sin(angle-0.2)*from_radius, 0, size)
	
	point3 = gmsh.model.geo.add_point(
		cos(angle+0.2)*from_radius+cos(angle)*(to_radius-from_radius), 
		sin(angle+0.2)*from_radius+sin(angle)*(to_radius-from_radius), 0, size)
	point4 = gmsh.model.geo.add_point(
		cos(angle-0.2)*from_radius+cos(angle)*(to_radius-from_radius), 
		sin(angle-0.2)*from_radius+sin(angle)*(to_radius-from_radius), 0, size)
	
	line1 = gmsh.model.geo.addLine(point1, point3)
	line2 = gmsh.model.geo.addLine(point2, point4)

	return line1, line2


def wheel(small_radius, large_radius, small_thickness, large_thickness,
		  n_axes, axes_thickness, size):
	center = gmsh.model.geo.add_point(0, 0, 0, size)

	inner_point1 = gmsh.model.geo.add_point(small_radius-small_thickness, 0, 0, size)
	inner_point2 = gmsh.model.geo.add_point(-small_radius+small_thickness, 0, 0, size)
	inner_circle = [gmsh.model.geo.add_circle_arc(inner_point1, center, inner_point2),
					gmsh.model.geo.add_circle_arc(inner_point2, center, inner_point1)]
	
	outer_point1 = gmsh.model.geo.add_point(large_radius, 0, 0, size)
	outer_point2 = gmsh.model.geo.add_point(-large_radius, 0, 0, size)
	outer_circle = [gmsh.model.geo.add_circle_arc(outer_point1, center, outer_point2),
					gmsh.model.geo.add_circle_arc(outer_point2, center, outer_point1)]
	
	axe_holes = [0]*n_axes
	for i in range(n_axes):
		angle = i/n_axes*2*pi
		next_angle = angle+2*pi/n_axes
		point1 = gmsh.model.geo.add_point(
			cos(angle+axes_thickness)*small_radius,
			sin(angle+axes_thickness)*small_radius,
			0, size
		)
		point2 = gmsh.model.geo.add_point(
			cos(next_angle-axes_thickness)*small_radius,
			sin(next_angle-axes_thickness)*small_radius,
			0, size
		)
		point3 = gmsh.model.geo.add_point(
			cos(angle+axes_thickness)*small_radius+cos(angle)*
				(large_radius-large_thickness-small_radius), 
			sin(angle+axes_thickness)*small_radius+sin(angle)*
				(large_radius-large_thickness-small_radius),
			0, size
		)
		point4 = gmsh.model.geo.add_point(
			cos(next_angle-axes_thickness)*small_radius+cos(next_angle)*
				(large_radius-large_thickness-small_radius), 
			sin(next_angle-axes_thickness)*small_radius+sin(next_angle)*
				(large_radius-large_thickness-small_radius),
			0, size
		)

		part1 = gmsh.model.geo.add_circle_arc(point1, center, point2)
		part2 = gmsh.model.geo.add_line(point2, point4)
		part3 = gmsh.model.geo.add_circle_arc(point4, center, point3)
		part4 = gmsh.model.geo.add_line(point3, point1)

		face = gmsh.model.geo.add_curve_loop([part1, part2, part3, part4])
		axe_holes[i] = face

	big_circle = gmsh.model.geo.add_curve_loop(outer_circle)
	smol_circle = gmsh.model.geo.add_curve_loop(inner_circle)

	gmsh.model.geo.add_plane_surface([big_circle, smol_circle] + axe_holes)


if __name__ == "__main__":
	gmsh.initialize()

	s = float(argv[1])

	wheel(small_radius=0.1,
		  large_radius=1,
		  small_thickness=0.05,
		  large_thickness=0.07,
		  n_axes=5,
		  axes_thickness=0.4,
		  size=s)

	gmsh.model.geo.synchronize()
	
	gmsh.model.mesh.generate()
	gmsh.model.mesh.refine()

	gmsh.write("../ProjectPreProcessor/data/mesh.msh")

	# gmsh.fltk.run()
	gmsh.finalize()