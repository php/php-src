--TEST--
Test readdir() function : usage variations - use file pointers
--FILE--
<?php
/* Prototype  : string readdir([resource $dir_handle])
 * Description: Read directory entry from dir_handle 
 * Source code: ext/standard/dir.c
 */

/*
 * Open a file pointer using fopen and pass to readdir() to test behaviour
 */

echo "*** Testing readdir() : usage variations ***\n";

// get a resource variable
var_dump($fp = fopen(__FILE__, "r"));
var_dump( readdir($fp) );

// get file length over 256 characters
<<<EOT
123456789012345678901234567890
123456789012345678901234567890
123456789012345678901234567890
123456789012345678901234567890
123456789012345678901234567890
EOT;
?>
===DONE===
--EXPECTF--
*** Testing readdir() : usage variations ***
resource(%d) of type (stream)

Warning: readdir(): %d is not a valid Directory resource in %s on line %d
bool(false)
===DONE===
