--TEST--
dom: Can we go without dom_set_old_ns?
--FILE--
<?php

$dom = new DOMDocument();
$dom->appendChild($element = $dom->createElement('xml:xml'));
$element->setAttribute('xmlns', 'http://www.w3.org/2000/xmlns/');
$element->setAttribute('xmlns:xml', 'http://www.w3.org/XML/1998/namespace');
echo $dom->saveXML();

$html = new DOMDocument();
$element->setAttribute('xmlns', 'http://www.w3.org/2000/xmlns/');
$element->setAttribute('xmlns:xml', 'http://www.w3.org/XML/1998/namespace');
$html->appendChild($element = $html->createElement('xml:html'));

echo $html->saveHTML();

$dom->documentElement->appendChild($dom->importNode($element));

echo $dom->saveXML();

// now test performance of 10000 namespaced elements. second run should not take more than 3x (safety margin) as long as first time.

$dom = new DOMDocument();
$root = $dom->createElementNS('http://www.w3.org/2000/xhtml', 'html');
$dom->appendChild($root);

$s = microtime(true);
for ($i = 0; $i < 10000; $i++) {
    $element = $dom->createElementNS('http://www.w3.org/2000/xhtml', 'p', 'Hello World');
    $root->appendChild($element);
}
$firstRun = microtime(true) - $s;

$s = microtime(true);
for ($i = 0; $i < 10000; $i++) {
    $element = $dom->createElementNS('http://www.w3.org/2000/xhtml', 'p', 'Hello World');
    $root->appendChild($element);
}
$secondRun = microtime(true) - $s;

if ($firstRun * 3 < $secondRun) {
    echo "FAIL; second run took more than 3 times as long as first run. Performance should be linear\n";
}

--EXPECTF--
<?xml version="1.0"?>
<xml:xml xmlns="http://www.w3.org/2000/xmlns/" xmlns:xml="http://www.w3.org/XML/1998/namespace"/>
<xml:html></xml:html>
<?xml version="1.0"?>
<xml:xml xmlns="http://www.w3.org/2000/xmlns/" xmlns:xml="http://www.w3.org/XML/1998/namespace"><xml:html/></xml:xml>
