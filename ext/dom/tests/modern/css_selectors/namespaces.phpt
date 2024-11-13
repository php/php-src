--TEST--
CSS Selectors - Namespaces
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<root>
    <container align="left"/>
    <only>
        <a xmlns="urn:a"/>
        <a xmlns="urn:a"/>
        <a xmlns="urn:b"/>
        <a xmlns=""/>
        <a/>
    </only>
</root>
XML);

$container = $dom->documentElement->firstElementChild;
$container->setAttribute("foo:bar", "baz");
$container->setAttributeNS("urn:a", "a:bar", "baz");

test_helper($dom, 'container[align]');
test_helper($dom, 'container[foo\\:bar]');
test_helper($dom, 'container[a\\:bar]');
test_helper($dom, 'container[bar]');

test_helper($dom, 'a:first-of-type');
test_helper($dom, 'a:last-of-type');

test_failure($dom, 'container[* | bar]');

?>
--EXPECT--
--- Selector: container[align] ---
<container align="left" foo:bar="baz" xmlns:a="urn:a" a:bar="baz"/>
--- Selector: container[foo\:bar] ---
<container align="left" foo:bar="baz" xmlns:a="urn:a" a:bar="baz"/>
--- Selector: container[a\:bar] ---
--- Selector: container[bar] ---
--- Selector: a:first-of-type ---
<a xmlns="urn:a"/>
<a xmlns="urn:b"/>
<a xmlns=""/>
--- Selector: a:last-of-type ---
<a xmlns="urn:a"/>
<a xmlns="urn:b"/>
<a/>
--- Selector: container[* | bar] ---
Code 12 Invalid selector (Selectors. Unexpected token: *)
