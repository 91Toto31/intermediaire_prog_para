#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 4;
    int matrix[16];

    if (rank == 0) {
        // Process 0 initializes the matrix as a 1D array
        for (int i = 0; i < n * n; ++i) {
            matrix[i] = i;
        }

        // Create MPI derived datatype for the lower triangular part
        int blocklengths[] = {1, 1, 1, 1};
        int displacements[] = {0, 4, 5, 8};
        MPI_Datatype blocktype;
        MPI_Type_indexed(4, blocklengths, displacements, MPI_INT, &blocktype);
        MPI_Type_commit(&blocktype);

        // Send the lower triangular part with a single MPI_Send call
        MPI_Send(matrix, 1, blocktype, 1, 0, MPI_COMM_WORLD);

        // Free the MPI datatype
        MPI_Type_free(&blocktype);
    } else if (rank == 1) {
        // Process 1 receives the lower triangular part with a single MPI_Recv call
        int received[4];
        MPI_Recv(received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Print the received data
        printf("Process 1 received: %d %d %d %d\n", received[0], received[1], received[2], received[3]);
    }

    MPI_Finalize();
    return 0;
}
