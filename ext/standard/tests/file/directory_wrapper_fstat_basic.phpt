--TEST--
Test function fstat() on directory wrapper 
--FILE--
<?php
$d = dirname(__FILE__);
$h = opendir($d);
var_dump(fstat($h));
closedir($h);
?>
===DONE===
--EXPECT--
bool(false)
===DONE===