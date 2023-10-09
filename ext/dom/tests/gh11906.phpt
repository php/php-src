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
echo "--- document output ---\n";
echo $doc->saveXML();
echo "--- \$test output ---\n";
echo $doc->saveXML($test), "\n";
$test->prepend($child);
echo "--- document output ---\n";
echo $doc->saveXML();
echo "--- \$test output ---\n";
echo $doc->saveXML($test), "\n";
$test->append($child);
?>
--EXPECT--
--- document output ---
<?xml version="1.0"?>
<container>
    
</container>
--- $test output ---
<foo><child/></foo>
--- document output ---
<?xml version="1.0"?>
<container>
    
</container>
--- $test output ---
<foo><child/></foo>
