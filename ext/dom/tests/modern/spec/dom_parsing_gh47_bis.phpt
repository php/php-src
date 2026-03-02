--TEST--
DOM-Parsing GH-47 bis (XML null namespaces need to be preserved)
--EXTENSIONS--
dom
--FILE--
<?php

// Adapted from https://github.com/web-platform-tests/wpt/blob/master/domparsing/XMLSerializer-serializeToString.html

$dom = Dom\XMLDocument::createFromString('<root xmlns=""><child xmlns=""/></root>');
echo $dom->saveXml(), "\n";

$dom = Dom\XMLDocument::createFromString('<root xmlns="urn:u1"><child xmlns="urn:u1"/></root>');
echo $dom->saveXml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns=""><child xmlns=""/></root>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:u1"><child xmlns="urn:u1"/></root>
