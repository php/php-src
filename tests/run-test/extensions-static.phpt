--TEST--
phpt EXTENSIONS directive - static extension is present
--EXTENSIONS--
standard
--FILE--
<?php
var_dump(extension_loaded('standard'));
?>
--EXPECT--
bool(true)
