--TEST--
Test mime_content_type() function : error 
--SKIPIF--
<?php if (!extension_loaded("mime_magic")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : string mime_content_type(string filename|resource stream)
 * Description: Return content-type for file 
 * Source code: ext/mime_magic/mime_magic.c
 * Alias to functions: 
 */

echo "*** Testing mime_content_type() : error ***\n";

// Zero arguments
echo "\n-- Testing mime_content_type() function with Zero arguments --\n";
var_dump( mime_content_type() );

//Test mime_content_type with one more than the expected number of arguments
echo "\n-- Testing mime_content_type() function with more than expected no. of arguments --\n";

$extra_arg = 10;
var_dump( mime_content_type("mime_test.jpg", $extra_arg) );


?>
===DONE===
--EXPECTF--
*** Testing mime_content_type() : error ***

-- Testing mime_content_type() function with Zero arguments --

Warning: mime_content_type() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing mime_content_type() function with more than expected no. of arguments --

Warning: mime_content_type() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
