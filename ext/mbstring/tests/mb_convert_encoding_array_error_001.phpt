--TEST--
Test mb_convert_encoding() function : array functionality with objects not supported
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_convert_encoding(["key" => new stdClass()], 'UTF-8');
?>
--EXPECTF--
Warning: mb_convert_encoding(): Object is not supported in %s on line %d
