# Recover FAT32 File System

This repository is a FAT32 file recovery tool.

_If you have accidentally deleted a file, you can use this tool recover to it, as long as you remember the name of the file!_

## Getting Started

1. Clone the repository

   ```
   $ git clone https://github.com/rvk007/Recover-FAT32-File-System.git
   ```

2. Install openssl package
   ```
   $ sudo apt install openssl
   ```

To compile, run the below command

```
$ make
```

To use this tool you need the binary file of the FAT32 disk. To know how to create a FAT32 disk click [here](https://github.com/rvk007/Recover-FAT32-File-System#create-a-fat32-disk).

## Usage

To see the meanings of all the flags of the tool just run `./file` command:

```
$ ./file
Usage: ./nyufile disk <options>
  -i                     Print the file system information.
  -l                     List the root directory.
  -r filename [-s sha1]  Recover a contiguous file.
  -R filename -s sha1    Recover a possibly non-contiguous file.
```

### Print the file system information

```
$ ./file <disk> -i
```

The output would look something like this:

```
$ ./file fat32.disk -i
Number of FATs = 2
Number of bytes per sector = 512
Number of sectors per cluster = 1
Number of reserved sectors = 32
```

### List the root directory

```
$ ./file <disk> -l
```

Shows the contents of the root directory along with its size and starting cluster in the file system. It also lists the total number of entries in the root directory.

```
$ ./file fat32.disk -l
DIR/ (size = 0, starting cluster = 3)
EMPTY (size = 0, starting cluster = 0)
HELLO.TXT (size = 13, starting cluster = 20)
LARGE.TXT (size = 4129, starting cluster = 7)
MEDIUM.TXT (size = 1549, starting cluster = 16)
MELLO.TXT (size = 15, starting cluster = 21)
MARGE.TXT (size = 0, starting cluster = 0)
Total number of entries = 7
```

### Recover a file

To recover a file, it must be marked as deleted in the file system. The steps to delete a file are be accessed [here](https://github.com/rvk007/Recover-FAT32-File-System#delete-a-file).
You can check if the file is deleted or not using the above command.

```
$ ./file fat32.disk -l
DIR/ (size = 0, starting cluster = 3)
EMPTY (size = 0, starting cluster = 0)
HELLO.TXT (size = 13, starting cluster = 20)
LARGE.TXT (size = 4129, starting cluster = 7)
MELLO.TXT (size = 15, starting cluster = 21)
MARGE.TXT (size = 0, starting cluster = 0)
Total number of entries = 6
```

As seen from the above two results `MEDIUM.TXT` file is deleted.
To recover a deleted file use the below command. Once the file is recovered it will return `<filename>: successfully recovered` and you can find the file again in the root directory by running the second command.

```
$ ./file <disk> -r <filename>
```

For example, the below command can be used to recover the `MEDIUM.TXT` file:

```
$ ./file fat32.disk -r MEDIUM.TXT
MEDIUM.TXT: successfully recovered
```

To verify if the file is recovered or not, use the `-l` flag again:

```
$ ./file fat32.disk -l
DIR/ (size = 0, starting cluster = 3)
EMPTY (size = 0, starting cluster = 0)
HELLO.TXT (size = 13, starting cluster = 20)
LARGE.TXT (size = 4129, starting cluster = 7)
MEDIUM.TXT (size = 1549, starting cluster = 16)
MELLO.TXT (size = 15, starting cluster = 21)
MARGE.TXT (size = 0, starting cluster = 0)
Total number of entries = 7
```

As seen from the above output, the number of entries is increased to 7 again and `MEDIUM.TXT` is showing in the root directory.

If the given filename does not exist the tool will print `<filename>: file not found`.

```
$ ./file fat32.disk -r FILE.TXT
FILE.TXT: file not found
```

### Handle ambiguous file recovery requests

When a file is deleted, its first character is changed to `0xE5`. Now, if there are multiple files with whose filenames differ only by the first character then it becomes tricky for the tool to recover the file. These filenames can be like HELLO.TXT and MELLO.TXT or LARGE.TXT and MARGE.TXT.
In that case, the tool will return `<filename>: multiple candidates found` and even if the file exists it won't be able to recover it.

```
$ ./file fat32.disk -r LARGE.TXT
LARGE.TXT: multiple candidates found
```

### Recover a file with SHA-1 hash

To avoid this situation we can provide SHA-1 hash of the file. SHA-1 hash is 160-bit fingerprint of a file. It is unique for each file and depends on the file content. To get the SHA-1 hash of your file run the below command:

```
$ sha1sum <filename>
```

For example, to generate the SHA-1 hash of LARGE.TXT file, run the below command:

```
$ sha1sum /mnt/disk/LARGE.TXT
92a18d61b3591abd6db3b01ce75240ac045a15cf  /mnt/disk/LARGE.TXT
```

With SHA-1 hash the tool can exactly identify the given file even if there are multiple files with similar names. To recover a file with SHA-1 hash, use the below command:

```
$ ./file <disk> -r <filename> -s <SHA-1>
```

For example, to recover LARGE.TXT

```
$ ./file fat32.disk -r LARGE.TXT -s 92a18d61b3591abd6db3b01ce75240ac045a15cf
LARGE.TXT: successfully recovered with SHA-1
```

## Create a FAT32 disk

To create a FAT32 disk, follow these steps:

### Create an empty file of a certain size

Firstly, create an empty file which contains zeros. Let's create a 256KB file named fat32.disk:

```
$ dd if=/dev/zero of=fat32.disk bs=256k count=1
```

### Format the disk with FAT32

Use `mkfs.fat` command to convert the above file to FAT32 file system.

```
$ mkfs.fat -F 32 -f 2 -S 512 -s 1 -R 32 fat32.disk
```

Meanings of each of these options:

```
-F: type of FAT (FAT12, FAT16, or FAT32)
-f: number of FATs
-S: number of bytes per sector
-s: number of sectors per cluster
-R: number of reserved sectors
```

### Verify the file system information

Use `fsck.fat` command to check the FAT file system details:

```
$ fsck.fat -v fat32.disk
fsck.fat 3.0.20 (12 Jun 2013)
fsck.fat 3.0.20 (12 Jun 2013)
Checking we can access the last sector of the filesystem
Warning: Filesystem is FAT32 according to fat_length and fat32_length fields,
  but has only 472 clusters, less than the required minimum of 65525.
  This may lead to problems on some systems.
Boot sector contents:
System ID "mkfs.fat"
Media byte 0xf8 (hard disk)
       512 bytes per logical sector
       512 bytes per cluster
        32 reserved sectors
First FAT starts at byte 16384 (sector 32)
         2 FATs, 32 bit entries
      2048 bytes per FAT (= 4 sectors)
Root directory start at cluster 2 (arbitrary size)
Data area starts at byte 20480 (sector 40)
       472 data clusters (241664 bytes)
32 sectors/track, 64 heads
         0 hidden sectors
       512 sectors total
Checking for unused clusters.
Checking free cluster summary.
fat32.disk: 0 files, 1/472 clusters
```

### Mount the file system

The file system can be mounted to any empty folder. So, let's create a folder `/mnt/disk` and mount `fat32.disk` to that point.

```
$ sudo mkdir /mnt/disk
$ sudo mount -o umask=0 fat32.disk /mnt/disk
```

### Creating files and directory

Once the file system is mounted, new files and folders can be created, edited and deleted.
As not all the OS support long filenames, only 8.3 filename format is considered as anaming convention.

The filename can be of atmost eight characters which can be only uppercase letters, number and the following special characters `` ! # $ % & ' ( ) - @ ^ _ ` { } ~ `` . The filename is followed by an optional '.' and at most three characters.

```
$ echo "Hello, world." > /mnt/disk/HELLO.TXT
$ mkdir /mnt/disk/DIR
$ touch /mnt/disk/EMPTY
```

After writing to the disk, make sure to run the below command to _flush the file system cache_:

```
$ sync
```

### Unmount the file system

Once all the changes are made to the disk, you can unmount the disk:

```
$ sudo umount /mnt/disk
```

### Examine the file system

To examine the file system use `xxd` command. To examine the root directory you can give a range using `-s`(starting offset) and `-l`(length) flags.

```
$ xxd -s 20480 -l 96 fat32.disk
00005000: 4845 4c4c 4f20 2020 5458 5420 0000 0000  HELLO   TXT ....
00005010: 6e53 6e53 0000 0000 6e53 0300 0e00 0000  nSnS....nS......
00005020: 4449 5220 2020 2020 2020 2010 0000 0000  DIR        .....
00005030: 6e53 6e53 0000 0000 6e53 0400 0000 0000  nSnS....nS......
00005040: 454d 5054 5920 2020 2020 2020 0000 0000  EMPTY       ....
00005050: 6e53 6e53 0000 0000 6e53 0000 0000 0000  nSnS....nS......
```

### Delete a file

```
$ sudo mount -o umask=0 fat32.disk /mnt/disk
$ ls /mnt/disk
DIR  EMPTY  HELLO.TXT  LARGE.TXT  MARGE.TXT  MEDIUM.TXT  MELLO.TXT
$ rm /mnt/disk2/MEDIUM.TXT
$ ls /mnt/disk2
DIR  EMPTY  HELLO.TXT  LARGE.TXT  MARGE.TXT  MELLO.TXT
$ sudo umount /mnt/disk
```

## Contact/ Getting Help

If you need any help or want to report a bug, raise an issue in the repo.
