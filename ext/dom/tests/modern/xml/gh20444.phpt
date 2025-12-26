--TEST--
GH-20444 (Dom\XMLDocument::C14N() seems broken compared to DOMDocument::C14N())
--EXTENSIONS--
dom
--FILE--
<?php

$xml = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<test:root xmlns:test="http://example.com/dummy/ns">
    <test:a xmlns:test="http://example.com/dummy/ns"/>
    <test:b test:kind="123">abc</test:b>
</test:root>
EOF;

$d = \Dom\XMLDocument::createFromString($xml);
var_dump($d->C14N(true));

$xml = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<ns1:root xmlns:ns1="http://example.com/dummy/ns">
    <ns1:a/>
    <ns1:b>
        <ns1:c>123</ns1:c>
    </ns1:b>
</ns1:root>
EOF;

$d = \Dom\XMLDocument::createFromString($xml);
var_dump($d->C14N());

?>
--EXPECT--
string(128) "<test:root xmlns:test="http://example.com/dummy/ns">
    <test:a></test:a>
    <test:b test:kind="123">abc</test:b>
</test:root>"
string(134) "<ns1:root xmlns:ns1="http://example.com/dummy/ns">
    <ns1:a></ns1:a>
    <ns1:b>
        <ns1:c>123</ns1:c>
    </ns1:b>
</ns1:root>"
