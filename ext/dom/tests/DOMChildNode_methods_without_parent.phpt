--TEST--
DOMChildNode methods without a parent
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
foreach (['before', 'after', 'replaceWith'] as $method) {
    echo "--- $method ---\n";
    $test->$method($child);
    echo $doc->saveXML();
    echo $doc->saveXML($test), "\n";
}
?>
--EXPECT--
--- before ---
<?xml version="1.0"?>
<container>
    <child/>
</container>
<foo/>
--- after ---
<?xml version="1.0"?>
<container>
    <child/>
</container>
<foo/>
--- replaceWith ---
<?xml version="1.0"?>
<container>
    <child/>
</container>
<foo/>
