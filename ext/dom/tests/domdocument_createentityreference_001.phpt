--TEST--
DomDocument::CreateEntityReference() - Creates an entity reference with the appropriate name
--CREDITS--
Clint Priest @ PhpTek09
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
    $objDoc = new DomDocument();

    $objRef = $objDoc->createEntityReference('Test');
    echo $objRef->nodeName . "\n";
?>
--EXPECT--
Test
	
