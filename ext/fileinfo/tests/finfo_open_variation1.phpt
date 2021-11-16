--TEST--
Test finfo_open() function : variations in opening
--EXTENSIONS--
fileinfo
--FILE--
<?php
$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic';

echo "*** Testing finfo_open() : variations in opening ***\n";

// Calling finfo_open() with different options
var_dump( finfo_open( FILEINFO_MIME | FILEINFO_SYMLINK, $magicFile ) );
//var_dump( finfo_open( FILEINFO_COMPRESS | FILEINFO_PRESERVE_ATIME, $magicFile ) );
var_dump( finfo_open( FILEINFO_DEVICES | FILEINFO_RAW, $magicFile ) );

?>
--EXPECTF--
*** Testing finfo_open() : variations in opening ***
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
