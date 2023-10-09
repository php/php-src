--TEST--
Bug #80602 (Segfault when using DOMChildNode::before())
--EXTENSIONS--
dom
--FILE--
<?php
declare(strict_types=1);

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before($target);
echo "1 ", $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before($target);
echo "2 ", $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before($doc->documentElement->lastChild);
echo "3 ", $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before($doc->documentElement->firstChild);
echo "4 ", $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before($target, $doc->documentElement->lastChild);
echo "5 ", $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before($doc->documentElement->lastChild, $target);
echo "6 ", $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before($target, $doc->documentElement->firstChild);
echo "7 ", $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before($doc->documentElement->firstChild, $target);
echo "8 ", $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before('bar','baz');
echo "9 ", $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before('bar','baz');
echo "10 ", $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before($target, 'bar','baz');
echo "11 ", $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before('bar', $target, 'baz');
echo "12 ", $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before('bar', 'baz', $target);
echo "13 ", $doc->saveXML($doc->documentElement).PHP_EOL;



$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before($target, 'bar','baz');
echo "14 ", $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before('bar', $target, 'baz');
echo "15 ", $doc->saveXML($doc->documentElement).PHP_EOL;

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before('bar', 'baz', $target);
echo "16 ", $doc->saveXML($doc->documentElement).PHP_EOL;



$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before('bar', $target, $doc->documentElement->lastChild);
echo "17 ", $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before($target, 'bar', $doc->documentElement->lastChild);
echo "18 ", $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->firstChild;
$target->before($target, $doc->documentElement->lastChild, 'bar');
echo "19 ", $doc->saveXML($doc->documentElement).PHP_EOL;




$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before('bar', $doc->documentElement->firstChild, $target);
echo "20 ", $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before($doc->documentElement->firstChild, 'bar', $target);
echo "21 ", $doc->saveXML($doc->documentElement).PHP_EOL;


$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before($doc->documentElement->firstChild, $target, 'bar');
echo "22 ", $doc->saveXML($doc->documentElement).PHP_EOL;

?>
--EXPECTF--
1 <a>foo<last/></a>
2 <a>foo<last/></a>
3 <a><last/>foo</a>
4 <a>foo<last/></a>
5 <a>foo<last/></a>
6 <a><last/>foo</a>
7 <a><last/>foo</a>
8 <a>foo<last/></a>
9 <a>barbazfoo<last/></a>
10 <a>foobarbaz<last/></a>
11 <a>foobarbaz<last/></a>
12 <a>barfoobaz<last/></a>
13 <a>barbazfoo<last/></a>
14 <a>foo<last/>barbaz</a>
15 <a>foobar<last/>baz</a>
16 <a>foobarbaz<last/></a>
17 <a>barfoo<last/></a>
18 <a>foobar<last/></a>
19 <a>foo<last/>bar</a>
20 <a>barfoo<last/></a>
21 <a>foobar<last/></a>
22 <a>foo<last/>bar</a>
