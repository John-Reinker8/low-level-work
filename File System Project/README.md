# Hey! I'm Filing Here

In this lab, I successfully implemented a 1 MiB ext2 file system, which has 2 directories (root and lost+found), 1 regular file (hello-world), and 1 symbolic link (hello -> hello_world)

## Building
Use the following commands to setup everything:

make                                    ### to compile the executable
./ext2-create                           ### to create cs111-base.img by running the executable
mkdir mnt                               ### to create a directory to mount the filesystem to
sudo mount -o loop cs111 -base.img mnt  ### to mount the filesystem

## Running
Use the following commands to check correctness:

ls -ain mnt/                            ### to check to see if the filesystem has been correctly created

## Cleaning up
Use the following commands when cleaning up the project:

sudo umount mnt                         ### to unmount the filesystem
rmdir mnt                               ### to remove the directory that the filesystem was mounted to
make clean                              ### removes all binary files

