--TEST--
GH-10234 (Setting DOMAttr::textContent results in an empty attribute value.)
--EXTENSIONS--
dom
--FILE--
<?php
$document = new DOMDocument();
$document->loadXML('<element attribute="value"/>');
$attribute = $document->documentElement->getAttributeNode('attribute');

var_dump($document->saveHTML());
var_dump($attribute->textContent);

$attribute->textContent = 'new value';
var_dump($attribute->textContent);
var_dump($document->saveHTML());

$attribute->textContent = 'hello & world';
var_dump($attribute->textContent);
var_dump($document->saveHTML());

$attribute->textContent = '<b>hi</b>';
var_dump($attribute->textContent);
var_dump($document->saveHTML());

$document->documentElement->textContent = 'hello & world';
var_dump($document->documentElement->textContent);
var_dump($document->saveHTML());

$document->documentElement->textContent = '<b>hi</b>';
var_dump($document->documentElement->textContent);
var_dump($document->saveHTML());

$document->documentElement->textContent = 'quote "test"';
var_dump($document->documentElement->textContent);
var_dump($document->saveHTML());

$document->documentElement->textContent = "quote 'test'";
var_dump($document->documentElement->textContent);
var_dump($document->saveHTML());
?>
--EXPECT--
string(38) "<element attribute="value"></element>
"
string(5) "value"
string(9) "new value"
string(42) "<element attribute="new value"></element>
"
string(13) "hello & world"
string(50) "<element attribute="hello &amp; world"></element>
"
string(9) "<b>hi</b>"
string(54) "<element attribute="&lt;b&gt;hi&lt;/b&gt;"></element>
"
string(13) "hello & world"
string(71) "<element attribute="&lt;b&gt;hi&lt;/b&gt;">hello &amp; world</element>
"
string(9) "<b>hi</b>"
string(75) "<element attribute="&lt;b&gt;hi&lt;/b&gt;">&lt;b&gt;hi&lt;/b&gt;</element>
"
string(12) "quote "test""
string(66) "<element attribute="&lt;b&gt;hi&lt;/b&gt;">quote "test"</element>
"
string(12) "quote 'test'"
string(66) "<element attribute="&lt;b&gt;hi&lt;/b&gt;">quote 'test'</element>
"
