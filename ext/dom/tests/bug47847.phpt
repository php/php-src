--TEST--
Bug #47847 (importNode loses the namespace of an XML element)
--EXTENSIONS--
dom
--XFAIL--
See https://github.com/php/php-src/pull/12308
--FILE--
<?php
$fromdom = new DOMDocument();
$fromdom->loadXML(<<<XML
<?xml version="1.0"?>
<ns:container xmlns:ns="http://php.net">
<ns:inner xmlns="http://php.net">
<ns:WATCH-MY-NAMESPACE xmlns=""/>
</ns:inner>
</ns:container>
XML);

$aDOM = new DOMDocument();
$imported = $aDOM->importNode($fromdom->documentElement->firstElementChild, true);
$aDOM->appendChild($imported);

echo $aDOM->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
<ns:inner xmlns="http://php.net" xmlns:ns="http://php.net">
<ns:WATCH-MY-NAMESPACE xmlns=""/>
</ns:inner>
