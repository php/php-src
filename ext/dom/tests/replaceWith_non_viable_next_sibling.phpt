--TEST--
replaceWith() with a non-viable next sibling
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <child>
        <alone/>
    </child>
</container>
XML);

$container = $doc->documentElement;
$child = $container->firstElementChild;
$alone = $child->firstElementChild;

$child->after($alone);
echo $doc->saveXML();
$child->replaceWith($alone);
echo $doc->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
<container>
    <child>
        
    </child><alone/>
</container>
<?xml version="1.0"?>
<container>
    <alone/>
</container>
