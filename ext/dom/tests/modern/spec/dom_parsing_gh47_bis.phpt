--TEST--
DOM-Parsing GH-47 bis (XML null namespaces need to be preserved)
--EXTENSIONS--
dom
--FILE--
<?php

// Adapted from https://github.com/web-platform-tests/wpt/blob/master/domparsing/XMLSerializer-serializeToString.html

$dom = DOM\XMLDocument::createFromString('<root xmlns=""><child xmlns=""/></root>');
echo $dom->saveXML(), "\n";

$dom = DOM\XMLDocument::createFromString('<root xmlns="urn:u1"><child xmlns="urn:u1"/></root>');
echo $dom->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns=""><child xmlns=""/></root>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:u1"><child xmlns="urn:u1"/></root>
