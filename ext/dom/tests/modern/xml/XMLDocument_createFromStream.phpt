--TEST--
Dom\XMLDocument::createFromStream() - from memory
--EXTENSIONS--
dom
--FILE--
<?php

$tmp = fopen("php://memory", "r+");
fwrite($tmp, "<root/>");
rewind($tmp);
$dom1 = Dom\XMLDocument::createFromStream($tmp);
rewind($tmp);
$dom2 = Dom\XMLDocument::createFromStream($tmp, "http://example.com");
fclose($tmp);

var_dump($dom1->documentURI);
var_dump($dom2->documentURI);

echo $dom1->saveXml(), "\n";
echo $dom2->saveXml(), "\n";

?>
--EXPECT--
string(11) "about:blank"
string(18) "http://example.com"
<?xml version="1.0" encoding="UTF-8"?>
<root/>
<?xml version="1.0" encoding="UTF-8"?>
<root/>
