--TEST--
Test finfo_file() function : basic functionality
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic';
$finfo = finfo_open( FILEINFO_MIME );

echo "*** Testing finfo_file() : basic functionality ***\n";

// Calling finfo_file() with all possible arguments
var_dump( finfo_file( $finfo, __FILE__) );
var_dump( finfo_file( $finfo, __FILE__, FILEINFO_CONTINUE ) );
var_dump( finfo_file( $finfo, $magicFile ) );
var_dump( finfo_file( $finfo, $magicFile.chr(0).$magicFile) );

?>
--EXPECTF--
*** Testing finfo_file() : basic functionality ***
string(28) "text/x-php; charset=us-ascii"
string(%d) "PHP script, ASCII text%A"
string(32) "text/plain; charset=unknown-8bit"

Warning: finfo_file(): Invalid path in %s%efinfo_file_basic.php on line %d
bool(false)
