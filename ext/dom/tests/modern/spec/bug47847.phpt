--TEST--
Bug #47847 (importNode loses the namespace of an XML element)
--EXTENSIONS--
dom
--FILE--
<?php
$fromdom = Dom\XMLDocument::createFromString(<<<XML
<?xml version="1.0"?>
<ns:container xmlns:ns="http://php.net">
<ns:inner xmlns="http://php.net">
<ns:WATCH-MY-NAMESPACE xmlns=""/>
</ns:inner>
</ns:container>
XML);

$aDOM = Dom\XMLDocument::createEmpty();
$imported = $aDOM->importNode($fromdom->documentElement->firstElementChild, true);
$aDOM->appendChild($imported);

echo $aDOM->saveXml(), "\n";

var_dump($aDOM->documentElement->firstElementChild->prefix);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<ns:inner xmlns:ns="http://php.net" xmlns="http://php.net">
<WATCH-MY-NAMESPACE/>
</ns:inner>
string(2) "ns"
