--TEST--
Document URI/URL
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root><child/></root>');
var_dump($dom->documentURI);
var_dump($dom->URL);

try {
    $dom->URL = NULL;
    var_dump($dom->documentURI);
    var_dump($dom->URL);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$dom->URL = "";
var_dump($dom->documentURI);
var_dump($dom->URL);

$dom->URL = "http://example.com/";
var_dump($dom->documentURI);
var_dump($dom->URL);

?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
Cannot assign null to property DOM\Document::$URL of type string
string(0) ""
string(0) ""
string(%d) "%s"
string(%d) "%s"
