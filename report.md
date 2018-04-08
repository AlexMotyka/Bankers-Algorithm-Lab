SOFE 3950U LAB 5 Banker's Algorithm Report

The Banker's algorithm is a resource allocation and deadlock avoidance algorithm, which simulates allocations of resources for processes. It’s goal is to ensure that it never allocates its resources in such a way where it can not serve other processes.

To prevent issues such as two processes trying to update the same variable at the same time we have three mutex locks. The first being mutexavail which is for the available number of resources, second being mutexalloc which is for the amount currently allocated to each customer and finally mutexneed which is for the remaining need of each customer.

When a resource is being requested the thread first acquires the resource allocation mutex lock and increments the allocation variable by how many resources it is requesting and ulock the mutex. Then it will acquire the mutex for the available amount of resources and decrement that variable by how many resources it requires and then unlock the mutex. Finally the thread will acquire need mutex and decrement how many resources it need and unlock the mutex.

After this is completed then the checkIfSafe function is called which will check if the transaction that is being attempted will leave the system in a safe state. This is where the banker's algorithm is executed and it returns whether the resource allocation leaves the system in a safe or unsafe state.

If checkIfSafe fails then the system is reverted back to the previous state, this is done as mentioned above by acquiring mutex locks to safely revert the variables to their original values. If it was Safe then it returns 0 and resource allocation is completed successfully.

For releasing resources the process is the same in terms of acquiring the mutex locks but different in that there is no check if the system is left in a safe state.
