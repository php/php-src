--TEST--
DomDocument::CreateEntityReference() - Creates an entity reference with the appropriate name
--CREDITS--
Clint Priest @ PhpTek09
--EXTENSIONS--
dom
--FILE--
<?php
    $objDoc = new DomDocument();

    $objRef = $objDoc->createEntityReference('Test');
    echo $objRef->nodeName . "\n";
?>
--EXPECT--
Test
