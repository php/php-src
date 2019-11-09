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
try {
    var_dump( readdir($fp) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
*** Testing readdir() : usage variations ***
resource(%d) of type (stream)
%d is not a valid Directory resource
