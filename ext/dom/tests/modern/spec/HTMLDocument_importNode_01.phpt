--TEST--
HTMLDocument::importNode() with namespace should not shift namespaces
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0"?>
<html>
    <body xmlns="http://www.w3.org/1999/xhtml">
        <h1>hello world.</h1>
        <p>test</p>
        <br/>
        <p>test 2</p>
        <default:p xmlns:default="http://www.w3.org/1999/xhtml" class="foo" id="import">namespace prefixed</default:p>
    </body>
</html>
XML);

// Note the HTMLDocument class!
$dom2 = DOM\HTMLDocument::createEmpty();
$imported = $dom2->importNode($dom->documentElement, true);
$dom2->appendChild($imported);

$body = $dom2->getElementsByTagName("body")[0];
$default_p = $body->lastElementChild;
var_dump($default_p->prefix);
var_dump($default_p->namespaceURI);

echo $dom2->saveXML();

?>
--EXPECT--
string(7) "default"
string(28) "http://www.w3.org/1999/xhtml"
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html>
    <body xmlns="http://www.w3.org/1999/xhtml">
        <h1>hello world.</h1>
        <p>test</p>
        <br />
        <p>test 2</p>
        <p xmlns:default="http://www.w3.org/1999/xhtml" class="foo" id="import">namespace prefixed</p>
    </body>
</html>
