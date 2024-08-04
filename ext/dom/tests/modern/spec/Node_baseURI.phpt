--TEST--
Node::$baseURI
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$html = $dom->appendChild($dom->createElement('html'));
var_dump($html->baseURI);

$dom = Dom\HTMLDocument::createFromFile(__DIR__ . "/../../empty.html", LIBXML_NOERROR);
var_dump($dom->documentElement->baseURI);

$dom = Dom\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<html>
    <head>
        <base href="http://example.com/">
    </head>
</html>
HTML);
var_dump($dom->documentElement->baseURI);

?>
--EXPECTF--
string(11) "about:blank"
string(%d) "file://%sempty.html"
string(19) "http://example.com/"
