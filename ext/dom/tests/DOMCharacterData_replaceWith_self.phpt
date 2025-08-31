--TEST--
DOMCharacterData::replaceWith() with itself
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument;
$dom->loadXML('<?xml version="1.0"?><container><![CDATA[Hello]]></container>');
$cdata = $dom->documentElement->firstChild;
$cdata->replaceWith($cdata);
echo $dom->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
<container><![CDATA[Hello]]></container>
