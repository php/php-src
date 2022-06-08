--TEST--
Bug #80602 (Segfault when using DOMChildNode::before())
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
declare(strict_types=1);

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before('bar', 'baz', 'cat');
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before($doc->documentElement->firstChild, 'baz', 'cat');
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before('bar', $doc->documentElement->firstChild, 'baz');
echo $doc->saveXML($doc->documentElement).PHP_EOL;

// if `$doc->documentElement->firstChild` is the last parameter, it will cause segment fault.
$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before('bar', $doc->documentElement->firstChild, 'baz', $doc->documentElement->firstChild);
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before($doc->documentElement->firstChild, $doc->documentElement->firstChild, $doc->documentElement->firstChild);
echo $doc->saveXML($doc->documentElement).PHP_EOL;

?>
--EXPECTF--
<a>foobarbazcat<last/></a>
<a>foobazcat<last/></a>
<a>barfoobaz<last/></a>
<a>barfoobazfoo<last/></a>
<a>foofoofoo<last/></a>
