--TEST--
GH-10234 (Setting DOMAttr::textContent results in an empty attribute value.)
--EXTENSIONS--
dom
--FILE--
<?php
$document = new DOMDocument();
$document->loadXML('<element attribute="value"/>');
$attribute = $document->documentElement->getAttributeNode('attribute');

echo "-- Attribute tests --\n";

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

$attribute->textContent = 'quote "test"';
var_dump($attribute->textContent);
var_dump($document->saveHTML());

$attribute->textContent = "quote 'test'";
var_dump($attribute->textContent);
var_dump($document->saveHTML());

$attribute->textContent = "quote '\"test\"'";
var_dump($attribute->textContent);
var_dump($document->saveHTML());

echo "-- Document element tests --\n";

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
-- Attribute tests --
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
string(12) "quote "test""
string(45) "<element attribute='quote "test"'></element>
"
string(12) "quote 'test'"
string(45) "<element attribute="quote 'test'"></element>
"
string(14) "quote '"test"'"
string(57) "<element attribute="quote '&quot;test&quot;'"></element>
"
-- Document element tests --
string(13) "hello & world"
string(74) "<element attribute="quote '&quot;test&quot;'">hello &amp; world</element>
"
string(9) "<b>hi</b>"
string(78) "<element attribute="quote '&quot;test&quot;'">&lt;b&gt;hi&lt;/b&gt;</element>
"
string(12) "quote "test""
string(69) "<element attribute="quote '&quot;test&quot;'">quote "test"</element>
"
string(12) "quote 'test'"
string(69) "<element attribute="quote '&quot;test&quot;'">quote 'test'</element>
"
