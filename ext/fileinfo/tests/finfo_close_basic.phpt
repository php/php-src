--TEST--
Test finfo_close() function : basic functionality
--EXTENSIONS--
fileinfo
--FILE--
<?php
echo "*** Testing finfo_close() : basic functionality ***\n";

$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic';

$finfo = finfo_open( FILEINFO_MIME, $magicFile );
var_dump( $finfo );

// Calling finfo_close() with all possible arguments
var_dump( finfo_close($finfo) );

$finfo = new finfo( FILEINFO_MIME, $magicFile );
var_dump( $finfo );
unset( $finfo );

?>
--EXPECTF--
*** Testing finfo_close() : basic functionality ***
object(finfo)#%d (0) {
}

Deprecated: Function finfo_close() is deprecated since 8.5, as finfo objects are freed automatically in %s on line %d
bool(true)
object(finfo)#%d (%d) {
}
