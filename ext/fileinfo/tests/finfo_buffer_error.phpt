--TEST--
Test finfo_buffer() function : error conditions 
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : string finfo_buffer(resource finfo, char *string [, int options [, resource context]])
 * Description: Return infromation about a string buffer. 
 * Source code: ext/fileinfo/fileinfo.c
 * Alias to functions: 
 */

$magicFile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'magic';

echo "*** Testing finfo_buffer() : error conditions ***\n";

$finfo = finfo_open( FILEINFO_NONE, $magicFile );

//Test finfo_buffer with one more than the expected number of arguments
echo "\n-- Testing finfo_buffer() function with more than expected no. of arguments --\n";

$context = stream_context_get_default();
$extra_arg = 10;
var_dump( finfo_buffer($finfo, "foobar", FILEINFO_MIME, $context, $extra_arg) );

// Testing finfo_buffer with one less than the expected number of arguments
echo "\n-- Testing finfo_buffer() function with less than expected no. of arguments --\n";

var_dump( finfo_buffer($finfo) );

?>
===DONE===
--EXPECTF--
*** Testing finfo_buffer() : error conditions ***

-- Testing finfo_buffer() function with more than expected no. of arguments --

Warning: finfo_buffer() expects at most 4 parameters, 5 given in %s on line %d
bool(false)

-- Testing finfo_buffer() function with less than expected no. of arguments --

Warning: finfo_buffer() expects at least 2 parameters, 1 given in %s on line %d
bool(false)
===DONE===
