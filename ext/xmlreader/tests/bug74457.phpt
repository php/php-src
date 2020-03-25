--TEST--
XMLReader: Bug #74457 Wrong reflection on XMLReader::expand
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) die("skip xmlreader extension not available"); ?>
--FILE--
<?php
$rm = new ReflectionMethod(XMLReader::class, 'expand');
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());
?>
--EXPECT--
int(1)
int(0)
