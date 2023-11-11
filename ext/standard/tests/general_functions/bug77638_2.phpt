--TEST--
Bug #77638 (var_export'ing certain class instances segfaults)
--EXTENSIONS--
ffi
--FILE--
<?php
var_export(FFI::new('int'));
?>
--EXPECT--
\FFI\CData::__set_state(array(
))
