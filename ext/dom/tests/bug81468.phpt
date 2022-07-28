--TEST--
Bug #81468 (Inconsistent default namespace inheritance)
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$dom = new \DOMDocument();
$dom
  ->appendChild($dom->createElementNS('some:namespace', 'foo'))
  ->appendChild($dom->createElement('bar'));
echo ($xml = $dom->saveXML());

$xpath = new \DOMXPath($dom);
$xpath->registerNamespace('n', 'some:namespace');
echo count($xpath->query('/n:foo/bar')) . " should be 0\n";
echo count($xpath->query('/n:foo/n:bar')) . " should be 1\n\n";

$dom = new \DOMDocument();
$dom->loadXml($xml);
echo ($xml = $dom->saveXML());

$xpath = new \DOMXPath($dom);
$xpath->registerNamespace('n', 'some:namespace');
echo count($xpath->query('/n:foo/bar')) . " should be 0\n";
echo count($xpath->query('/n:foo/n:bar')) . " should be 1\n\n";


$dom = new \DOMDocument();
$dom
  ->appendChild($dom->createElementNS('some:namespace', 'foo'))
  ->appendChild($dom->createElementNS('some:namespace_test', 'bar'));
echo ($xml = $dom->saveXML());

$xpath = new \DOMXPath($dom);
$xpath->registerNamespace('n', 'some:namespace');
$xpath->registerNamespace('m', 'some:namespace_test');
echo count($xpath->query('/n:foo/bar')) . " should be 0\n";
echo count($xpath->query('/n:foo/m:bar')) . " should be 1\n\n";

$dom = new \DOMDocument();
$dom->loadXml($xml);
echo ($xml = $dom->saveXML());

$xpath = new \DOMXPath($dom);
$xpath->registerNamespace('n', 'some:namespace');
$xpath->registerNamespace('m', 'some:namespace_test');
echo count($xpath->query('/n:foo/bar')) . " should be 0\n";
echo count($xpath->query('/n:foo/m:bar')) . " should be 1\n\n";

$dom = new \DOMDocument();
$dom
  ->appendChild($dom->createElement('bar'))
  ->appendChild($dom->createElementNS('some:namespace', 'foo'));
echo ($xml = $dom->saveXML());

$xpath = new \DOMXPath($dom);
$xpath->registerNamespace('n', 'some:namespace');
echo count($xpath->query('/bar/n:foo')) . " should be 1\n";
echo count($xpath->query('/bar/foo')) . " should be 0\n\n";

?>
--EXPECT--
<?xml version="1.0"?>
<foo xmlns="some:namespace"><bar/></foo>
0 should be 0
1 should be 1

<?xml version="1.0"?>
<foo xmlns="some:namespace"><bar/></foo>
0 should be 0
1 should be 1

<?xml version="1.0"?>
<foo xmlns="some:namespace"><bar xmlns="some:namespace_test"/></foo>
0 should be 0
1 should be 1

<?xml version="1.0"?>
<foo xmlns="some:namespace"><bar xmlns="some:namespace_test"/></foo>
0 should be 0
1 should be 1

<?xml version="1.0"?>
<bar><foo xmlns="some:namespace"/></bar>
1 should be 1
0 should be 0
