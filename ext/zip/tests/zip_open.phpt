--TEST--
zip_open() function
--EXTENSIONS--
zip
--FILE--
<?php
$zip = zip_open(__DIR__."/test_procedural.zip");

echo is_resource($zip) ? "OK" : "Failure";

?>
--EXPECTF--
Deprecated: Function zip_open() is deprecated in %s on line %d
OK
