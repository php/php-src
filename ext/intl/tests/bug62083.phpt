--TEST--
Bug #62083: grapheme_extract() leaks memory
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
$arr1 = array();
var_dump(grapheme_extract(-1, -1, -1,-1, $arr1));
--EXPECT--
bool(false)
