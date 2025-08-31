--TEST--
Bug #73053 (XML reader with setSchema now fails under 5.6.25)
--EXTENSIONS--
xmlreader
--SKIPIF--
<?php
if (LIBXML_VERSION === 20904) die('skip fails with libxml 2.9.4');
?>
--FILE--
<?php
$xmlfile = __DIR__ . DIRECTORY_SEPARATOR . 'bug73053.xml';
$xsdfile = __DIR__ . DIRECTORY_SEPARATOR . 'bug73053.xsd';

$xml = new XMLReader();
var_dump($xml->open($xmlfile, null, LIBXML_PARSEHUGE));
$xml->setSchema($xsdfile);
while($xml->read());
?>
--EXPECT--
bool(true)
