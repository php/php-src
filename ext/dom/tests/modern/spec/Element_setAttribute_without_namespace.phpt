--TEST--
Element::setAttribute() without namespace
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . "/dump_attr.inc";

$dom = Dom\HTMLDocument::createEmpty();
$container = $dom->appendChild($dom->createElement("container"));

$attrs = [];

function setAttribute($container, string $name, string $value): Dom\Attr {
    $container->setAttribute($name, $value);
    return $container->getAttributeNode($name);
}

$attrs[] = setAttribute($container, "foo:bar", "&hello1");
echo $dom->saveHtml($container), "\n";
$attrs[] = setAttribute($container, "foo:bar", "&hello2");
echo $dom->saveHtml($container), "\n";
$attrs[] = setAttribute($container, "bar", "&hello3");
echo $dom->saveHtml($container), "\n";
$attrs[] = setAttribute($container, "xmlns", "&hello4");
echo $dom->saveHtml($container), "\n";
$attrs[] = setAttribute($container, "XMLns", "&hello5");
echo $dom->saveHtml($container), "\n";
$attrs[] = setAttribute($container, "BAR", "&hello6");
echo $dom->saveHtml($container), "\n";

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
<container foo:bar="&amp;hello2" bar="&amp;hello3" xmlns="&amp;hello5"></container>
<container foo:bar="&amp;hello2" bar="&amp;hello6" xmlns="&amp;hello5"></container>
Attr: foo:bar
NULL
string(7) "foo:bar"
NULL
Attr: foo:bar
NULL
string(7) "foo:bar"
NULL
Attr: bar
NULL
string(3) "bar"
NULL
Attr: xmlns
NULL
string(5) "xmlns"
NULL
Attr: xmlns
NULL
string(5) "xmlns"
NULL
Attr: bar
NULL
string(3) "bar"
NULL
