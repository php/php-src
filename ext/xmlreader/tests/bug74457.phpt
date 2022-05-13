--TEST--
XMLReader: Bug #74457 Wrong reflection on XMLReader::expand
--EXTENSIONS--
xmlreader
--FILE--
<?php
$rm = new ReflectionMethod(XMLReader::class, 'expand');
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());
?>
--EXPECT--
int(1)
int(0)
