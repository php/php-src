--TEST--
noscript behaviour
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Parsing ---\n";

$dom = Dom\HTMLDocument::createFromString("<!doctype html><html><body><noscript><p>hi</p></noscript></body></html>", Dom\HTML_NO_DEFAULT_NS);
var_dump($dom->documentElement->textContent);
var_dump($dom->documentElement->namespaceURI);
echo $dom->saveHtml(), "\n";
echo $dom->saveXml(), "\n";

echo "--- Modifying the text content: tag ---\n";

$xpath = new Dom\XPath($dom);
$noscript = $xpath->query("//noscript")[0];
$noscript->textContent = "<p>bye</p>";
echo $dom->saveHtml(), "\n";
echo $dom->saveXml(), "\n";

echo "--- Modifying the text content: trick ---\n";

$noscript->textContent = "<!-- </noscript> -->";
echo $dom->saveHtml(), "\n";
echo $dom->saveXml(), "\n";

?>
--EXPECT--
--- Parsing ---
string(2) "hi"
NULL
<!DOCTYPE html><html><head></head><body><noscript><p>hi</p></noscript></body></html>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html><head/><body><noscript><p>hi</p></noscript></body></html>
--- Modifying the text content: tag ---
<!DOCTYPE html><html><head></head><body><noscript>&lt;p&gt;bye&lt;/p&gt;</noscript></body></html>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html><head/><body><noscript>&lt;p&gt;bye&lt;/p&gt;</noscript></body></html>
--- Modifying the text content: trick ---
<!DOCTYPE html><html><head></head><body><noscript>&lt;!-- &lt;/noscript&gt; --&gt;</noscript></body></html>
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html><head/><body><noscript>&lt;!-- &lt;/noscript&gt; --&gt;</noscript></body></html>
