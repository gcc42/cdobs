# cdobs
Database object storage backend for Ceph

### Dependencies
Install the following packages on Fedora for SQLite3:
```bash
sudo dnf install sqlite-devel sqlite-tcl sqlite-jdbc
```

### Build
```bash
# clone the repo
git clone https://github.com/forumulator/cdobs
mkdir cdobs/build
cd cdobs/build/
# Run cmake to create makefile
cmake .. 
make
```
This will create the executable `cdobs/build/src/cdobs`

### Currently supported operations
1. Init
2. Create Bucket

### Example Usage
```bash
# initialize the db file
./cdobs init
# Create a bucket named b1
./cdobs create_bucket b1
```
