--TEST--
Test finfo_close() function : error conditions
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : resource finfo_close(resource finfo)
 * Description: Close fileinfo resource.
 * Source code: ext/fileinfo/fileinfo.c
 * Alias to functions:
 */

echo "*** Testing finfo_close() : error conditions ***\n";

echo "\n-- Testing finfo_close() function with wrong resource type --\n";
$fp = fopen( __FILE__, 'r' );
var_dump( finfo_close( $fp ) );

?>
===DONE===
--EXPECTF--
*** Testing finfo_close() : error conditions ***

-- Testing finfo_close() function with wrong resource type --

Warning: finfo_close(): supplied resource is not a valid file_info resource in %s on line %d
bool(false)
===DONE===
