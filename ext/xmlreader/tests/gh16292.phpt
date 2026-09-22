--TEST--
GH-16292 (Segmentation fault in ext/xmlreader/php_xmlreader.c:1282)
--EXTENSIONS--
dom
xmlreader
--FILE--
<?php

$character_data = new DOMCharacterData();
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book>new book</book></books>';
$reader = new XMLReader();
$reader->XML($xmlstring);
while ($reader->read()) {
    if ($reader->localName == "book") {
        var_dump($reader->expand($character_data));
    }
}

?>
--EXPECTF--
Warning: XMLReader::expand(): Couldn't fetch DOMCharacterData in %s on line %d
bool(false)

Warning: XMLReader::expand(): Couldn't fetch DOMCharacterData in %s on line %d
bool(false)
