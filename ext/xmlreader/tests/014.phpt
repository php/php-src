--TEST--
XMLReader: libxml2 XML Reader, set read-only node value test
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php
/* $Id$ */
$filename = dirname(__FILE__) . '/_014.xml';

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book num="1" idx="2">book1</book></books>';
file_put_contents($filename, $xmlstring);

$reader = new XMLReader();
if (!$reader->open($filename)) {
    exit();
}

// Only go through
while ($reader->read()) {
    if ($reader->nodeType != XMLREADER::END_ELEMENT) {
        if ($reader->nodeType == XMLREADER::ELEMENT && $reader->name == 'book') {
            // Try to write to a node
            $reader->value = 'new node value';
        }
        if ($reader->nodeType == XMLREADER::ELEMENT && $reader->hasAttributes) {
            // Try to write to a attribute
            $attr = $reader->moveToFirstAttribute();
            $reader->value = 'new index value';
        }
    }
}
$reader->close();
unlink($filename);
?>
===DONE===
--EXPECTF--
Warning: main(): Cannot write to read-only property in %s on line %d

Warning: main(): Cannot write to read-only property in %s on line %d
===DONE===

