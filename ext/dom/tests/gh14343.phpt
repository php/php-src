--TEST--
GH-14343 (Memory leak in xml and dom)
--EXTENSIONS--
dom
--FILE--
<?php
$aDOM = new DOMDocument();
$fromdom = new DOMDocument();
$fromdom->loadXML('<data xmlns:ai="http://test.org" ai:attr="namespaced" />');
$attr= $fromdom->firstChild->attributes->item(0);
$att = $aDOM->importNode($attr);
echo $aDOM->saveXML($att);
?>
--EXPECT--
 ai:attr="namespaced"
