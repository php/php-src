--TEST--
Test finfo_set_flags() function : basic functionality
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
$magicFile = __DIR__ . DIRECTORY_SEPARATOR . 'magic私はガラスを食べられます';
$finfo = finfo_open( FILEINFO_MIME, $magicFile );

echo "*** Testing finfo_set_flags() : basic functionality ***\n";

var_dump( finfo_set_flags( $finfo, FILEINFO_NONE ) );
var_dump( finfo_set_flags( $finfo, FILEINFO_SYMLINK ) );

finfo_close( $finfo );

// OO way
$finfo = new finfo( FILEINFO_NONE, $magicFile );
var_dump( $finfo->set_flags( FILEINFO_MIME ) );

?>
--EXPECT--
*** Testing finfo_set_flags() : basic functionality ***
bool(true)
bool(true)
bool(true)
