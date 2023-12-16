--TEST--
DOM\XMLDocument::documentURI
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromFile(__DIR__ . "/test foo.xml", LIBXML_NOERROR);
var_dump($dom->documentURI);

?>
--EXPECT--
string(66) "file:///home/niels/php-src/ext/dom/tests/modern/xml/test%20foo.xml"
