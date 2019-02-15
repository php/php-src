--TEST--
Test finfo_close() function : error conditions
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : resource finfo_close(resource finfo)
 * Description: Close fileinfo resource.
 * Source code: ext/fileinfo/fileinfo.c
 * Alias to functions:
 */

echo "*** Testing finfo_close() : error conditions ***\n";

$magicFile = dirname( __FILE__ ) . DIRECTORY_SEPARATOR . 'magic';
$finfo = finfo_open( FILEINFO_MIME, $magicFile );
$fp = fopen( __FILE__, 'r' );

echo "\n-- Testing finfo_close() function with Zero arguments --\n";
var_dump( finfo_close() );

echo "\n-- Testing finfo_close() function with more than expected no. of arguments --\n";
var_dump( finfo_close( $finfo, '10') );

echo "\n-- Testing finfo_close() function with wrong resource type --\n";
var_dump( finfo_close( $fp ) );

?>
===DONE===
--EXPECTF--
*** Testing finfo_close() : error conditions ***

-- Testing finfo_close() function with Zero arguments --

Warning: finfo_close() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing finfo_close() function with more than expected no. of arguments --

Warning: finfo_close() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

-- Testing finfo_close() function with wrong resource type --

Warning: finfo_close(): supplied resource is not a valid file_info resource in %s on line %d
bool(false)
===DONE===
