--TEST--
Bug #77638 (var_export'ing certain class instances segfaults)
--SKIPIF--
<?php
if (!extension_loaded('ffi')) die('skip ffi extension not available');
?>
--FILE--
<?php
var_export(FFI::new('int'));
?>
--EXPECT--
FFI\CData::__set_state(array(
))
