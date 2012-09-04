--TEST--
Test finfo_open() function : error functionality 
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); 
if(substr(PHP_OS, 0, 3) != 'WIN' )
  die("skip Not Valid for Linux");
?>
--FILE--
<?php
/* Prototype  : resource finfo_open([int options [, string arg]])
 * Description: Create a new fileinfo resource. 
 * Source code: ext/fileinfo/fileinfo.c
 * Alias to functions: 
 */

$magicFile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'magic';

echo "*** Testing finfo_open() : error functionality ***\n";

// on 5_4, this line generates additional warning messages that 5_3 does not. functionally, it should be fine (should pass)
var_dump( finfo_open( FILEINFO_MIME, 'foobarfile' ) );
var_dump( finfo_open( array(), $magicFile ) );
var_dump( finfo_open( FILEINFO_MIME, $magicFile, 'extraArg' ) );
var_dump( finfo_open( PHP_INT_MAX - 1, $magicFile ) );
var_dump( finfo_open( 'foobar' ) );

var_dump( new finfo('foobar') );

?>
===DONE===
--EXPECTF--
*** Testing finfo_open() : error functionality ***

Warning: finfo_open(%sfoobarfile): failed to open stream: No such file or directory in %sfinfo_open_error-win32.php on line %d

Warning: finfo_open(%sfoobarfile): failed to open stream: No such file or directory in %sfinfo_open_error-win32.php on line %d

Warning: finfo_open(): Failed to load magic database at '%sfoobarfile'. in %sfinfo_open_error-win32.php on line %d
bool(false)

Warning: finfo_open() expects parameter 1 to be long, array given in %sfinfo_open_error-win32.php on line %d
bool(false)

Warning: finfo_open() expects at most 2 parameters, 3 given in %sfinfo_open_error-win32.php on line %d
bool(false)
resource(6) of type (file_info)

Warning: finfo_open() expects parameter 1 to be long, string given in %sfinfo_open_error-win32.php on line %d
bool(false)

Warning: finfo::finfo() expects parameter 1 to be long, string given in %sfinfo_open_error-win32.php on line %d
NULL
===DONE===
