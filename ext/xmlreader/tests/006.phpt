--TEST--
XMLReader: libxml2 XML Reader, moveToElement
--EXTENSIONS--
xmlreader
--FILE--
<?php

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book num="1"></book><test /></books>';

$reader = new XMLReader();

$reader->XML($xmlstring);

// 2 read to get on the 2nd node
$reader->read();
$reader->read();

if ($reader->nodeType != XMLReader::END_ELEMENT) {
    if ($reader->nodeType == XMLReader::ELEMENT && $reader->hasAttributes) {
        $attr = $reader->moveToFirstAttribute();
        if ($reader->moveToElement()) {
            if ($reader->name == 'book') {
                echo "ok\n";
            }
        }
    }
}

$reader->close();
?>
--EXPECT--
ok
