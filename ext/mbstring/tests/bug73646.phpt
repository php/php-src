--TEST--
Bug #73646 (mb_ereg_search_init null pointer dereference)
--SKIPIF--
<?php
require 'skipif.inc';
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
var_dump(mb_ereg_search_init(NULL));
?>
--EXPECT--
bool(true)
