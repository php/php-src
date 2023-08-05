--TEST--
replaceWith() without a parent
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <child/>
</container>
XML);

$container = $doc->documentElement;
$child = $container->firstElementChild;

$test = $doc->createElement('foo');
$test->replaceWith($child);
echo $doc->saveXML();
echo $doc->saveXML($test);
?>
--EXPECT--
<?xml version="1.0"?>
<container>
    <child/>
</container>
<foo/>
