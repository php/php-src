--TEST--
Test finfo_open() function : error functionality 
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); 
--FILE--
<?php
/* Prototype  : resource finfo_open([int options [, string arg]])
 * Description: Create a new fileinfo resource. 
 * Source code: ext/fileinfo/fileinfo.c
 * Alias to functions: 
 */

$magicFile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'magic';

echo "*** Testing finfo_open() : error functionality ***\n";

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

Warning: finfo_open(%sfoobarfile): failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(%sfoobarfile): failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(): Failed to load magic database at '%sfoobarfile'. in %s on line %d
bool(false)

Warning: finfo_open() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Warning: finfo_open() expects at most 2 parameters, 3 given in %s on line %d
bool(false)

Notice: finfo_open(): Warning: using regular magic file `%s' in %s on line %d
resource(%d) of type (file_info)

Warning: finfo_open() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Warning: finfo::finfo() expects parameter 1 to be long, string given in %s on line %d
NULL
===DONE===
