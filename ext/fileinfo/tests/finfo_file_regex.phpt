--TEST--
Test finfo_file() function : regex rules
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); 
?>
--FILE--
<?php
/**
 * Works with the unix file command:
 * $ file -m magic resources/test.awk
 * resources/test.awk: awk script, ASCII text
 */
$magicFile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'magic';
$finfo = finfo_open( FILEINFO_MIME, $magicFile );

echo "*** Testing finfo_file() : regex rules ***\n";

// Calling finfo_file() with all possible arguments
$file = __DIR__ . '/resources/test.awk';
var_dump( finfo_file( $finfo, $file ) );
var_dump( finfo_file( $finfo, $file, FILEINFO_CONTINUE ) );

?>
===DONE===
--EXPECTF--
*** Testing finfo_file() : regex rules ***
string(28) "text/plain; charset=us-ascii"
string(22) "awk script, ASCII text"
===DONE===
