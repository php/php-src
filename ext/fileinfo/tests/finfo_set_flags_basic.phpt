--TEST--
Test finfo_set_flags() function : basic functionality
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool finfo_set_flags(resource finfo, int options)
 * Description: Set libmagic configuration options.
 * Source code: ext/fileinfo/fileinfo.c
 * Alias to functions:
 */

$magicFile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'magic';
$finfo = finfo_open( FILEINFO_MIME, $magicFile );

echo "*** Testing finfo_set_flags() : basic functionality ***\n";

var_dump( finfo_set_flags( $finfo, FILEINFO_NONE ) );
var_dump( finfo_set_flags( $finfo, FILEINFO_SYMLINK ) );
var_dump( finfo_set_flags() );

finfo_close( $finfo );

// OO way
$finfo = new finfo( FILEINFO_NONE, $magicFile );
var_dump( $finfo->set_flags( FILEINFO_MIME ) );
var_dump( $finfo->set_flags() );

?>
===DONE===
--EXPECTF--
*** Testing finfo_set_flags() : basic functionality ***
bool(true)
bool(true)

Warning: finfo_set_flags() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)
bool(true)

Warning: finfo::set_flags() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
===DONE===
