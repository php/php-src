--TEST--
Returning a Dom\Node from Dom\XPath callback
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root/>');
$xpath = new Dom\XPath($dom);
$xpath->registerPhpFunctionNs('urn:x', 'test', fn() => $dom->createElement('foo'));
$xpath->registerNamespace('x', 'urn:x');
$test = $xpath->query('x:test()');
var_dump($test[0]->nodeName);

?>
--EXPECT--
string(3) "foo"
