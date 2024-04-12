--TEST--
Bug #81468 (Inconsistent default namespace inheritance)
--EXTENSIONS--
dom
--FILE--
<?php

// Note: the original expectation from https://bugs.php.net/bug.php?id=81468 isn't *entirely* right

$dom = DOM\XMLDocument::createEmpty();
$dom
  ->appendChild($dom->createElementNS('some:namespace', 'foo'))
  ->appendChild($dom->createElement('bar'));
echo ($xml = $dom->saveXML()), "\n";

$xpath = new DOM\XPath($dom);
$xpath->registerNamespace('n', 'some:namespace');
echo "/n:foo/bar -> ", count($xpath->query('/n:foo/bar')), "\n";
echo "/n:foo/n:bar -> ", count($xpath->query('/n:foo/n:bar')), "\n";

//

$dom = DOM\XMLDocument::createFromString($xml);
echo ($xml = $dom->saveXML()), "\n";

$xpath = new DOM\XPath($dom);
$xpath->registerNamespace('n', 'some:namespace');
echo "/n:foo/bar -> ", count($xpath->query('/n:foo/bar')), "\n";
echo "/n:foo/n:bar -> ", count($xpath->query('/n:foo/n:bar')), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<foo xmlns="some:namespace"><bar xmlns=""/></foo>
/n:foo/bar -> 1
/n:foo/n:bar -> 0
<?xml version="1.0" encoding="UTF-8"?>
<foo xmlns="some:namespace"><bar xmlns=""/></foo>
/n:foo/bar -> 1
/n:foo/n:bar -> 0
