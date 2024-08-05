# LZWArchiver
## Build
```
cd build
cmake ..
make
```
## Run tests
```
./tests
```
## Run executable
```
./application [command and arguments]
```
## Pattern matching can be used when working with files. '*' means 0 or more arbitary characters while '?' is exactly one character.  
## Commands
### Create archive:
```
./application ZIP [archive path] [files/patterns]
```
#### Create archive named "zipped" in current directory with all files from directory ../files. Quotes should be used otherwise directory would not be traversed recursively:
```
./application ZIP ./zipped "../files/*" 
```
### Unzip archive:
```
./application UNZIP [unzip location] [archive location] [files/patterns to be unzipped]
```
### Unzip all files from archive named "zipped" in directory /unzipped:
```
./application UNZIP ./unzipped zipped "*"
```
### Check if archive was damaged using checksums(CRC32):
```
./application EC [archive location]
```
### Refresh files that were modified:
```
./application REFRESH [archive location] [files/patterns]
```
### Show information about level of compression of files in archive:
```
./application INFO [archive location]
```
