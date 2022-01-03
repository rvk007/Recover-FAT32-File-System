# Recover FAT32 File System

This repository is a FAT32 file recovery tool.

If you have accidentally deleted a file, you can use this tool recover it, as long as you remember its name!

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

## Usage

To use this tool you need the binary file of your FAT32 disk.

### Show the file system information.

```
$ ./file <disk> -i
```

### List the contents of root directory

```
$ ./file <disk> -l
```

Shows the contents of root directory along with its size and starting cluster in the file system. It also lists the total number of entries in the root directory.

### Recover a file

```
$ ./file <disk> -r <filename>
```

This project can recover any contiguosly allocated file present in the root directory of FAT32 disk.
Once the file is recovered it will return `<filename>: successfully recovered` and you can find the file again in the root directory by running the second command. If the given filename does not exists the tool will print `<filename>: file not found`.

If there are multiple files with whose file names differ only by first character, then it becomes tricky for the tool to recover such a file.
Such file names can be HELLO.TXT and JELLO.TXT. In that case the tool will return `<filename>: multiple candidates found` and even if the file exists it won't be recoved.
To avoid this situation use the below command.

### Recover a file with SHA-1 hash

```
$ ./file <disk> -r <filename> -s <SHA-1>
```

SHA-1 hash is 160-bit fingerprint of a file. It is unique for each file and depends on the file content. To get the SHA-1 hash of your file run the below command:
`sha1sum <filename>`.

With SHA-1 hash the tool can exactly identify the given file even if there are multiple files with similar names.

## Help

Run `./file` and it will print the complete usage of the tool.

## Have a question?
