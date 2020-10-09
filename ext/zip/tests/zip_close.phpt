--TEST--
zip_close() function
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$zip = zip_open(__DIR__."/test_procedural.zip");
if (!is_resource($zip)) die("Failure");
zip_close($zip);
echo "OK";

?>
--EXPECTF--
Deprecated: Function zip_open() is deprecated in %s on line %d

Deprecated: Function zip_close() is deprecated in %s on line %d
OK
