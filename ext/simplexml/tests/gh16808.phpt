--TEST--
GH-16808 (Segmentation fault in RecursiveIteratorIterator->current() with a xml element input)
--EXTENSIONS--
simplexml
--FILE--
<?php
$sxe = new SimpleXMLElement("<root />");
$test = new RecursiveIteratorIterator($sxe);
var_dump($test->current());
?>
--EXPECT--
NULL
