--TEST--
Dom\HTMLDocument::createFromStream() - from memory
--EXTENSIONS--
dom
--FILE--
<?php

$tmp = fopen("php://memory", "w+");
fwrite($tmp, "<!DOCTYPE html><html><p>Hello world</p></html>");
rewind($tmp);
$dom1 = Dom\HTMLDocument::createFromStream($tmp);
rewind($tmp);
$dom2 = Dom\HTMLDocument::createFromStream($tmp, "http://example.com");
fclose($tmp);

var_dump($dom1->documentURI);
var_dump($dom2->documentURI);

echo $dom1->saveHtml(), "\n";
echo $dom2->saveHtml(), "\n";

?>
--EXPECT--
string(11) "about:blank"
string(18) "http://example.com"
<!DOCTYPE html><html><head></head><body><p>Hello world</p></body></html>
<!DOCTYPE html><html><head></head><body><p>Hello world</p></body></html>
