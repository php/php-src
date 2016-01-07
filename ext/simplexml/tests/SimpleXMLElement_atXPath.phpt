--TEST--
SimpleXML: atXPath method
--SKIPIF--
<?php
if (!extension_loaded("simplexml")) print "skip SimpleXML not present";
if (!extension_loaded("libxml")) print "skip LibXML not present";
?>
--FILE--
<?php
$xml = file_get_contents(dirname(__FILE__) . '/book.xml');
$simpleXML = simplexml_load_string($xml);

echo $simpleXML->atXPath('wrong-xpath-query') . PHP_EOL;

echo $simpleXML->atXPath('book/title') . PHP_EOL;
echo $simpleXML->atXPath('book/author') . PHP_EOL;
?>
--EXPECT--
The Grapes of Wrath
John Steinbeck
