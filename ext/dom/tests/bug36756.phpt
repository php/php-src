--TEST--
Bug #36756 (DOMDocument::removeChild corrupts node)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

/* Node is preserved from removeChild */
$dom = new DOMDocument();
$dom->loadXML('<root><child/></root>');
$xpath = new DOMXpath($dom);
$node = $xpath->query('/root')->item(0);
echo $node->nodeName . "\n";
$dom->removeChild($GLOBALS['dom']->firstChild);
echo "nodeType: " . $node->nodeType . "\n";

/* Node gets destroyed during removeChild */
$dom->loadXML('<root><child/></root>');
$xpath = new DOMXpath($dom);
$node = $xpath->query('//child')->item(0);
echo $node->nodeName . "\n";
$GLOBALS['dom']->removeChild($GLOBALS['dom']->firstChild);

echo "nodeType: " . $node->nodeType . "\n";

?>
--EXPECTF--
root
nodeType: 1
child

Warning: Couldn't fetch DOMElement. Node no longer exists in %sbug36756.php on line %d

Notice: Undefined property:  DOMElement::$nodeType in %sbug36756.php on line %d
nodeType:
