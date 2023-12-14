--TEST--
Bug #47847 (importNode loses the namespace of an XML element)
--EXTENSIONS--
dom
--FILE--
<?php
$fromdom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0"?>
<ns:container xmlns:ns="http://php.net">
<ns:inner xmlns="http://php.net">
<ns:WATCH-MY-NAMESPACE xmlns=""/>
</ns:inner>
</ns:container>
XML);

$aDOM = DOM\XMLDocument::createEmpty();
$imported = $aDOM->importNode($fromdom->documentElement->firstElementChild, true);
$aDOM->appendChild($imported);

echo $aDOM->saveXML();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<ns:inner xmlns="http://php.net" xmlns:ns="http://php.net">
<ns:WATCH-MY-NAMESPACE xmlns=""/>
</ns:inner>
