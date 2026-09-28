--TEST--
GH-12171 (SimpleXML property-proxy equality must resolve iterator nodes)
--EXTENSIONS--
simplexml
--FILE--
<?php
$xml = simplexml_load_string('<root><first/><second/><third/></root>');
echo "first==second: ";
var_dump($xml->first == $xml->second);
echo "first==first: ";
var_dump($xml->first == $xml->first);
echo "first==root: ";
var_dump($xml->first == $xml);
$a = $xml->first;
$b = $xml->first;
echo "proxies equal: ";
var_dump($a == $b);
?>
--EXPECT--
first==second: bool(false)
first==first: bool(true)
first==root: bool(false)
proxies equal: bool(true)
