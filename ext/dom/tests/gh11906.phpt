--TEST--
GH-11906 (prepend without children after creating fragment results in segfault)
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
$test->append($child);
echo $doc->saveXML();
echo $doc->saveXML($test), "\n";
$test->prepend($child);
echo $doc->saveXML();
echo $doc->saveXML($test), "\n";
$test->append($child);
?>
--EXPECT--
<?xml version="1.0"?>
<container>
    
</container>
<foo><child/></foo>
<?xml version="1.0"?>
<container>
    
</container>
<foo><child/></foo>
