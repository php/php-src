--TEST--
XMLReader: libxml2 XML Reader, read-only element values can not be modified
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php
// Set up test data in a new file
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book num="1" idx="2">book1</book></books>';
$filename = __DIR__ . '/_014.xml';
file_put_contents($filename, $xmlstring);

// Load test data into a new XML Reader
$reader = new XMLReader();
if (!$reader->open($filename)) {
    exit('XML could not be read');
}

// Parse the data
while ($reader->read()) {
    if ($reader->nodeType != XMLREADER::END_ELEMENT) {
        // Find a node to try modifying
        if ($reader->nodeType == XMLREADER::ELEMENT && $reader->name == 'book') {
            // Try to set the value of the element from book1 to movie1
            $reader->value = 'movie1';
            // Try to set the value of the first "num" attribute from "1" to "num attribute 1"
            $attr = $reader->moveToFirstAttribute();
            $reader->value = 'num attribute 1';
            // Try to set the name of the first attribute from "num" to "number"
            $reader->name = 'number';
        }
    }
}

// clean up
$reader->close();
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__.'/_014.xml');
?>
--EXPECTF--
Warning: main(): Cannot write to read-only property in %s on line %d

Warning: main(): Cannot write to read-only property in %s on line %d

Warning: main(): Cannot write to read-only property in %s on line %d
===DONE===
