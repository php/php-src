--TEST--
Test finfo_open() function : basic functionality
--EXTENSIONS--
fileinfo
--FILE--
<?php
$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic';

echo "*** Testing finfo_open() : basic functionality ***\n";

// Calling finfo_open() with different options
var_dump( finfo_open( FILEINFO_MIME, $magicFile ) );
var_dump( finfo_open( FILEINFO_NONE, $magicFile ) );
var_dump( finfo_open( FILEINFO_SYMLINK, $magicFile ) );
//var_dump( finfo_open( FILEINFO_COMPRESS, $magicFile ) );
var_dump( finfo_open( FILEINFO_DEVICES, $magicFile ) );
var_dump( finfo_open( FILEINFO_CONTINUE, $magicFile ) );
var_dump( finfo_open( FILEINFO_PRESERVE_ATIME, $magicFile ) );
var_dump( finfo_open( FILEINFO_RAW, $magicFile ) );

// OO interface to finfo
var_dump( new finfo( FILEINFO_MIME, $magicFile ) );
var_dump( new finfo() );

?>
--EXPECTF--
*** Testing finfo_open() : basic functionality ***
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
