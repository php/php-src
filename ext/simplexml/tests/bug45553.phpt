--TEST--
Bug #45553 (Using XPath to return values for attributes with a namespace does not work)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml =<<<XML
<xml xmlns:a="http://a">
    <data a:label="I am A" label="I am Nothing">test1</data>
    <a:data a:label="I am a:A" label="I am a:Nothing">test2</a:data>
</xml>
XML;

$x = simplexml_load_string($xml);
$x->registerXPathNamespace("a", "http://a");

$atts = $x->xpath("/xml/data/@a:label");
echo $atts[0] . "\n";
$atts = $x->xpath("/xml/a:data");
echo $atts[0]->attributes() . "\n";
$atts = $x->xpath("/xml/a:data/@a:label");
echo $atts[0] . "\n";
$atts = $x->xpath("/xml/a:data/@label");
echo $atts[0] . "\n";
$atts = $x->xpath("/xml/data/@label");
echo $atts[0] . "\n";
?>
===DONE===
--EXPECT--
I am A
I am a:Nothing
I am a:A
I am a:Nothing
I am Nothing
===DONE===
	
