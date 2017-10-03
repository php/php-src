--TEST--
XMLReader: libxml2 XML Reader, read-only values can not be set
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php
$reader = new XMLReader();
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book num="1" idx="2">book1</book></books>';
$filename = dirname(__FILE__) . '/_014.xml';
file_put_contents($filename, $xmlstring);
if (!$reader->open($filename)) {
    exit('XML could not be read');
}
while ($reader->read()) {
    if ($reader->nodeType != XMLREADER::END_ELEMENT) {
        if ($reader->nodeType == XMLREADER::ELEMENT && $reader->name == 'book') {
            // Try to set the value of the node from book1 to movie1
            $reader->value = 'movie1';
        }
    }
}
$reader->close();
unlink($filename);
?>
===DONE===
--EXPECTF--
Warning: main(): Cannot write to read-only property in %s on line %d
===DONE===

