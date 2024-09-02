# Min Cost Max Flow

[CSES Task Assignment](https://cses.fi/problemset/task/2129)

The idea we can create a network having source, N employees, N tasks and 1 target. The construction of the network is simple. Each employee connects edge from the source. The edge from source has flow capacity 1 (employee gets only one task) and cost 0. Each task is connected to the target with flow capacity 1 and cost 0. Then if employee `i` spends on task `j` time `t`, construct edge from employee `i` to task `j`, set flow 1 and set cost `t`.

Then min cost max flow algorithm can be run on the network from source to target looking for flow of size N. The total time spent is a total cost. The employee task matching can be read from residues of their edges. The residue of value zero means the task was assigned to the employee.
