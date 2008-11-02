--TEST--
Test finfo_file() function : basic functionality 
--FILE--
<?php
/* Prototype  : string finfo_file(resource finfo, char *file_name [, int options [, resource context]])
 * Description: Return information about a file. 
 * Source code: ext/fileinfo/fileinfo.c
 * Alias to functions: 
 */

$magicFile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'magic';
$finfo = finfo_open( FILEINFO_MIME );

echo "*** Testing finfo_file() : basic functionality ***\n";

// Calling finfo_file() with all possible arguments
var_dump( finfo_file( $finfo, __FILE__) );
var_dump( finfo_file( $finfo, __FILE__, FILEINFO_CONTINUE ) );
var_dump( finfo_file( $finfo, $magicFile ) );

?>
===DONE===
--EXPECTF--
*** Testing finfo_file() : basic functionality ***
string(10) "text/x-php"
string(15) "PHP script text"
string(27) "text/plain; charset=unknown"
===DONE===
