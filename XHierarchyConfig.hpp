#ifndef XHIERARCHYCONFIG_HPP
#define XHIERARCHYCONFIG_HPP


// traverse iteratively instead of recursively
#define TRAVERSE_ITERATIVE	// <-- comment this out or in

// traverse near node first
#define TRAVERSE_ORDERED	// <-- comment this out or in

// type of node ids. maximum triangle count per scene is 67.108.864 for 32-bit int and 288.230.376.151.711.744 for 64-bit long :)
#if 1	// <-- set this to 0 or 1
	typedef unsigned long x_node_child_id_t;
#else
	typedef unsigned int x_node_child_id_t;
#endif


#endif

