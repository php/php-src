--TEST--
Test finfo_close() function : basic functionality
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : resource finfo_close(resource finfo)
 * Description: Close fileinfo resource.
 * Source code: ext/fileinfo/fileinfo.c
 * Alias to functions:
 */

echo "*** Testing finfo_close() : basic functionality ***\n";

$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic';

$finfo = finfo_open( FILEINFO_MIME, $magicFile );
var_dump( $finfo );

// Calling finfo_close() with all possible arguments
var_dump( finfo_close($finfo) );

$finfo = new finfo( FILEINFO_MIME, $magicFile );
var_dump( $finfo );
unset( $finfo );

?>
===DONE===
--EXPECTF--
*** Testing finfo_close() : basic functionality ***
resource(%d) of type (file_info)
bool(true)
object(finfo)#%d (%d) {
}
===DONE===
