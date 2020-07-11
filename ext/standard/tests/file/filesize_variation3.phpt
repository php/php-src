--TEST--
Test filesize() function: usage variations - file size after truncate
--FILE--
<?php
$file_path = __DIR__;

echo "*** Testing filesize(): usage variations ***\n";
$filename =  $file_path."/filesize_variation3.tmp";
$file_handle = fopen($filename, "w");
fwrite($file_handle, str_repeat("Hello,World ", 1000) ); // create file of size 12000 bytes
fclose($file_handle);

echo "-- Testing filesize() after truncating the file to a new length --\n";
// truncate the file created earlier in subdir, the size of the file is 12000bytes
// truncate the same file, in the loop , each time with the decrement in size by 1200 bytes,
//  until -1200bytes size
for($size = filesize($filename); $size>=-1200; $size-=1200) {
    $file_handle = fopen($filename, "r+");
    try {
        var_dump( ftruncate($file_handle, $size) );
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    fclose($file_handle);
    var_dump( filesize($filename) );
    clearstatcache();
}

?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/filesize_variation3.tmp");
?>
--EXPECT--
*** Testing filesize(): usage variations ***
-- Testing filesize() after truncating the file to a new length --
bool(true)
int(12000)
bool(true)
int(10800)
bool(true)
int(9600)
bool(true)
int(8400)
bool(true)
int(7200)
bool(true)
int(6000)
bool(true)
int(4800)
bool(true)
int(3600)
bool(true)
int(2400)
bool(true)
int(1200)
bool(true)
int(0)
ftruncate(): Argument #2 ($size) must be greater than or equal to 0
int(0)
