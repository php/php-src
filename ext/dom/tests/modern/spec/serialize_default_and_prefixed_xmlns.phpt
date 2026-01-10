--TEST--
Serialize default and prefixed xmlns on elements
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<root>
    <a:child xmlns:a="urn:a" xmlns="urn:default">
        <default/>
    </a:child>
</root>
XML);

echo $dom->saveXml();

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root>
    <a:child xmlns:a="urn:a" xmlns="urn:default">
        <default/>
    </a:child>
</root>
