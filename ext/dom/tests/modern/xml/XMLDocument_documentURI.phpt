--TEST--
DOM\XMLDocument::documentURI
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromFile(__DIR__ . "/test foo.xml", LIBXML_NOERROR);
var_dump($dom->documentURI);

?>
--EXPECTF--
string(%d) "file://%stest%20foo.xml"
