--TEST--
noscript behaviour
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Parsing ---\n";

$dom = DOM\HTMLDocument::createFromString("<!doctype html><html><body><noscript><p>hi</p></noscript></body></html>", DOM\HTML_NO_DEFAULT_NS);
var_dump($dom->documentElement->textContent);
echo $dom->saveHTML(), "\n";
echo $dom->saveXML(), "\n";

echo "--- Modifying the text content: tag ---\n";

$xpath = new DOMXPath($dom);
$noscript = $xpath->query("//noscript")[0];
$noscript->textContent = "<p>bye</p>";
echo $dom->saveHTML(), "\n";
echo $dom->saveXML(), "\n";

echo "--- Modifying the text content: trick ---\n";

$noscript->textContent = "<!-- </noscript> -->";
echo $dom->saveHTML(), "\n";
echo $dom->saveXML(), "\n";

?>
--EXPECT--
--- Parsing ---
string(2) "hi"
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
