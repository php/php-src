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
--EXPECT--
OK
