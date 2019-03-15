--TEST--
Test finfo_open() function : error functionality
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc');
--FILE--
<?php
/* Prototype  : resource finfo_open([int options [, string arg]])
 * Description: Create a new fileinfo resource.
 * Source code: ext/fileinfo/fileinfo.c
 * Alias to functions:
 */

$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic';

echo "*** Testing finfo_open() : error functionality ***\n";

var_dump( finfo_open( FILEINFO_MIME, 'foobarfile' ) );
var_dump( finfo_open( array(), $magicFile ) );
var_dump( finfo_open( FILEINFO_MIME, $magicFile, 'extraArg' ) );
var_dump( finfo_open( PHP_INT_MAX - 1, $magicFile ) );
var_dump( finfo_open( 'foobar' ) );

try {
    var_dump( new finfo('foobar') );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
===DONE===
--EXPECTF--
*** Testing finfo_open() : error functionality ***

Warning: finfo_open(%sfoobarfile): failed to open stream: No such file or directory in %sfinfo_open_error.php on line 12

Warning: finfo_open(%sfoobarfile): failed to open stream: No such file or directory in %sfinfo_open_error.php on line 12

Warning: finfo_open(): Failed to load magic database at '%sfoobarfile'. in %sfinfo_open_error.php on line 12
bool(false)

Warning: finfo_open() expects parameter 1 to be int, array given in %sfinfo_open_error.php on line 13
bool(false)

Warning: finfo_open() expects at most 2 parameters, 3 given in %sfinfo_open_error.php on line 14
bool(false)

Notice: finfo_open(): Warning: using regular magic file `%smagic' in %sfinfo_open_error.php on line 15
resource(6) of type (file_info)

Warning: finfo_open() expects parameter 1 to be int, string given in %sfinfo_open_error.php on line 16
bool(false)
finfo::finfo() expects parameter 1 to be int, string given
===DONE===
