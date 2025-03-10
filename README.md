# OpenMPI Demo

This is a repository for creating an OpenMPI base container with UCX and LSF.

# Docker Image

Shown below is the steps to create a docker image.

1. Clone this repository.  Shown below is an example of command.
   ```bash
   git clone https://github.com/WashU-IT-RIS/demo-openmpi.git
   ```
2. Change directory to the repository clone above.  Shown below is an example.
   ```bash
   cd demo-openmpi
   ```
3. Build a docker image.  I use my docker registry.
   Please replace `ghcr.io/washu-it-ris/base-openmpi:latest`
   with your docker image name.
   Shown below is an example command.
   ```bash
   docker build --tag ghcr.io/washu-it-ris/base-openmpi:latest .
   ```
4. Push to a docker registry.  Shown below is an example command.
   Please replace `ghcr.io/washu-it-ris/base-openmpi:latest`
   with your docker image name.
   ```bash
   docker push ghcr.io/washu-it-ris/base-openmpi:latest
   ```

# Test

## RIS Compute Cluster

Shown below are the steps to test it in the RIS Compute Cluster.

1. Login to a client node.  Shown below is an example.
   ```bash
   ssh compute1-client-1.ris.wustl.edu
   ```
2. Submit a LSF interactive job.
   Shown below is an example.
   Please replace `compute-ris` with your compute group.
   Please replace `ghcr.io/washu-it-ris/base-openmpi:latest)` with your docker image built above.
   ```bash
   LSF_DOCKER_NETWORK=host \
   LSF_DOCKER_IPC=host \
   LSF_DOCKER_SHM_SIZE=20G \
   bsub -q general-interactive \
        -Is -n 4 \
        -a "docker(ghcr.io/washu-it-ris/base-openmpi:latest)" \
        -R "affinity[core(1):distribute=pack] span[ptile=2]" \
        -G compute-ris \
        /bin/bash
   ``` 
3. Download a MPI test program source.  Shown below is an example. Please download it to your home directory or a directory that can be accessed from all RIS compute nodes.
   ```bash
   wget https://hpc-tutorials.llnl.gov/mpi/examples/mpi_ringtopo.f
   ```
4. Compile the test source code downloaded.  Shown below is an example.
   ```bash
   mpifort mpi_ringtopo.f
   ```
5. Test the MPI program.  Shown below is an example.
   The `--mca btl ^vader,tcp,openib,uct` is because by default for OpenMPI 4 and above enables build-in transports (BTLs).
   ```bash
   mpirun --mca btl ^vader,tcp,openib,uct \
          --mca pml ucx \
          -x UCX_NET_DEVICES=mlx5_0:1 \
          -x LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH \
          -np 4 \
          $(pwd)/a.out 
   ```
5. Shown below is an example of the output from above `mpirun` command.
   ```bash
    Task            0  communicated with tasks            3  &            1
    Task            3  communicated with tasks            2  &            0
    Task            1  communicated with tasks            0  &            2
    Task            2  communicated with tasks            1  &            3
   ```
