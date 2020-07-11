--TEST--
zip_open() function
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$zip = zip_open(__DIR__."/test_procedural.zip");

echo is_resource($zip) ? "OK" : "Failure";

?>
--EXPECTF--
Deprecated: Function zip_open() is deprecated in %s on line %d
OK
