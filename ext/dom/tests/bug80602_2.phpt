--TEST--
Bug #80602 (Segfault when using DOMChildNode::after())
--FILE--
<?php
declare(strict_types=1);

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after($target);
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after($target);
echo $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after($doc->documentElement->lastChild);
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after($doc->documentElement->firstChild);
echo $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after($target, $doc->documentElement->lastChild);
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after($doc->documentElement->lastChild, $target);
echo $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after($target, $doc->documentElement->firstChild);
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after($doc->documentElement->firstChild, $target);
echo $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after('bar','baz');
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after('bar','baz');
echo $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after($target, 'bar','baz');
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after('bar', $target, 'baz');
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after('bar', 'baz', $target);
echo $doc->saveXML($doc->documentElement).PHP_EOL;



$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after($target, 'bar','baz');
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after('bar', $target, 'baz');
echo $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after('bar', 'baz', $target);
echo $doc->saveXML($doc->documentElement).PHP_EOL;



$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after('bar', $target, $doc->documentElement->lastChild);
echo $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after($target, 'bar', $doc->documentElement->lastChild);
echo $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->after($target, $doc->documentElement->lastChild, 'bar');
echo $doc->saveXML($doc->documentElement).PHP_EOL;




$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after('bar', $doc->documentElement->firstChild, $target);
echo $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after($doc->documentElement->firstChild, 'bar', $target);
echo $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->after($doc->documentElement->firstChild, $target, 'bar');
echo $doc->saveXML($doc->documentElement).PHP_EOL;

?>
--EXPECTF--
<a>foo<last/></a>
<a>foo<last/></a>
<a>foo<last/></a>
<a><last/>foo</a>
<a>foo<last/></a>
<a><last/>foo</a>
<a><last/>foo</a>
<a>foo<last/></a>
<a>foobarbaz<last/></a>
<a>foo<last/>barbaz</a>
<a>foobarbaz<last/></a>
<a>barfoobaz<last/></a>
<a>barbazfoo<last/></a>
<a>foo<last/>barbaz</a>
<a>foobar<last/>baz</a>
<a>foobarbaz<last/></a>
<a>barfoo<last/></a>
<a>foobar<last/></a>
<a>foo<last/>bar</a>
<a>barfoo<last/></a>
<a>foobar<last/></a>
<a>foo<last/>bar</a>
