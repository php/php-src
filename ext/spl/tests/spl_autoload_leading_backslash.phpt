--TEST--
spl_autoload(): a leading "\" in the class name is ignored
--FILE--
<?php

set_include_path(__DIR__);
spl_autoload_extensions(".inc");

spl_autoload("DualIterator");
var_dump(class_exists("DualIterator", false));

spl_autoload("\\RecursiveDualIterator");
var_dump(class_exists("RecursiveDualIterator", false));

?>
--EXPECT--
bool(true)
bool(true)
