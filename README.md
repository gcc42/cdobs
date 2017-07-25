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

### Debugging
Debug mode build
```bash
cd cdobs/build/
cmake ..
make debug
```
Any output written to stream `dout` will output to console in this mode. 

### Currently supported operations
1. Init
2. Create Bucket
3. Delete Bucket
4. List Buckets
5. Put Object (Large and small objects)
6. Delete Object
7. List objects in bucket

### Example Usage
```bash
# initialize the db file
./cdobs init
# Create a bucket named b1
./cdobs bucket create b1
# List all buckets
./cdobs bucket
# Put object named makefile, with contents on Makefile in b1
./cdobs object put -b b1 -f Makefile makefile
# List objects in bucket b1
./cdobs object list -b b1
# Delete object makafile in bucket b1
./cdobs object delete -b b1 makefile
# Delete bucekt b1
./cdobs bucket delete b1
```
