--TEST--
Dom\XMLDocument::documentURI
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromFile(__DIR__ . "/test foo.xml", LIBXML_NOERROR);
var_dump($dom->documentURI);

?>
--EXPECTF--
string(%d) "file://%stest%sfoo.xml"
