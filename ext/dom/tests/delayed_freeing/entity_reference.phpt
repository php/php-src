--TEST--
Delayed freeing entity reference
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$entityRef = $doc->appendChild($doc->createElementNS('some:ns', 'container'))
    ->appendChild($doc->createEntityReference('nbsp'));
echo $doc->saveXML(), "\n";
$entityRef->parentNode->remove();
echo $doc->saveXML(), "\n";
var_dump($entityRef->parentNode);
var_dump($entityRef->nodeName);
var_dump($entityRef->textContent);

$doc = new DOMDocument;
$doc->loadXML(<<<'XML'
<?xml version="1.0"?>
<!DOCTYPE books [
<!ENTITY test "entity is only for test purposes">
]>
<div/>
XML);
$entityRef = $doc->documentElement->appendChild($doc->createEntityReference('test'));
echo $doc->saveXML(), "\n";
$entityRef->parentNode->remove();
unset($doc);
var_dump($entityRef->nodeName);
var_dump($entityRef->textContent);
?>
--EXPECTF--
<?xml version="1.0"?>
<container xmlns="some:ns">&nbsp;</container>

<?xml version="1.0"?>


Fatal error: Uncaught Error: Couldn't fetch DOMEntityReference. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
