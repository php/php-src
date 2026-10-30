--TEST--
GH-17500 (Segfault with requesting nodeName on nameless doctype)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadHTML("<!DOCTYPE>", LIBXML_NOERROR);
var_dump($doc->doctype->nodeName);

?>
--EXPECT--
string(0) ""
