--TEST--
Bug #55700 (XPath namespace prefix conflict, global registerNodeNS flag)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<prefix:root xmlns:prefix="urn:a" />');

$xp = new DOMXPath($doc, true);
$xp->registerNamespace('prefix', 'urn:b');

echo($xp->query('//prefix:root')->length . "\n");

$xp = new DOMXPath($doc, false);
$xp->registerNamespace('prefix', 'urn:b');

echo($xp->query('//prefix:root')->length . "\n");

var_dump($xp->registerNodeNamespaces);
$xp->registerNodeNamespaces = true;

var_dump($xp->registerNodeNamespaces);

echo($xp->query('//prefix:root')->length . "\n");

var_dump($xp);
?>
--EXPECT--
1
0
bool(false)
bool(true)
1
object(DOMXPath)#4 (2) {
  ["document"]=>
  string(22) "(object value omitted)"
  ["registerNodeNamespaces"]=>
  bool(true)
}
