--TEST--
Element::setAttribute() with namespace
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . "/dump_attr.inc";

$dom = DOM\HTMLDocument::createEmpty();
$container = $dom->appendChild($dom->createElementNS("urn:a", "container"));

$attrs = [];
$attrs[] = $container->setAttribute("foo:bar", "&hello1");
echo $dom->saveHTML($container), "\n";
$attrs[] = $container->setAttribute("foo:bar", "&hello2");
echo $dom->saveHTML($container), "\n";
$attrs[] = $container->setAttribute("bar", "&hello3");
echo $dom->saveHTML($container), "\n";
$attrs[] = $container->setAttribute("xmlns", "&hello4");
echo $dom->saveHTML($container), "\n";
$attrs[] = $container->setAttribute("XMLns", "&hello5");
echo $dom->saveHTML($container), "\n";
$attrs[] = $container->setAttribute("BAR", "&hello6");
echo $dom->saveHTML($container), "\n";

$container->setAttributeNode($dom->createAttributeNS("urn:b", "in:ns"));
$attrs[] = $container->setAttribute("in:ns", "&hello7");
echo $dom->saveHTML($container), "\n";

// Dump at the end to check whether they influenced each other
foreach ($attrs as $attr) {
    dumpAttr($attr);
}

?>
--EXPECT--
<container foo:bar="&amp;hello1"></container>
<container foo:bar="&amp;hello2"></container>
<container foo:bar="&amp;hello2" bar="&amp;hello3"></container>
<container foo:bar="&amp;hello2" bar="&amp;hello3" xmlns="&amp;hello4"></container>
<container foo:bar="&amp;hello2" bar="&amp;hello3" xmlns="&amp;hello4" XMLns="&amp;hello5"></container>
<container foo:bar="&amp;hello2" bar="&amp;hello3" xmlns="&amp;hello4" XMLns="&amp;hello5" BAR="&amp;hello6"></container>
<container foo:bar="&amp;hello2" bar="&amp;hello3" xmlns="&amp;hello4" XMLns="&amp;hello5" BAR="&amp;hello6" in:ns="&amp;hello7"></container>
Attr: foo:bar
string(0) ""
string(7) "foo:bar"
NULL
Attr: foo:bar
string(0) ""
string(7) "foo:bar"
NULL
Attr: bar
string(0) ""
string(3) "bar"
NULL
Attr: xmlns
string(0) ""
string(5) "xmlns"
NULL
Attr: XMLns
string(0) ""
string(5) "XMLns"
NULL
Attr: BAR
string(0) ""
string(3) "BAR"
NULL
Attr: ns
string(2) "in"
string(5) "in:ns"
string(5) "urn:b"
