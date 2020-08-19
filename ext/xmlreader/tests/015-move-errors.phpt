--TEST--
XMLReader: libxml2 XML Reader, Move cursor to a named attribute within a namespace, with invalid arguments
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php
// Set up test data in a new file
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books xmlns:ns1="http://www.ns1.namespace.org/" xmlns:ns2="http://www.ns2.namespace.org/"><book ns1:num="1" ns2:idx="2" ns1:idx="3" ns2:isbn="4">book1</book></books>';
$filename = __DIR__ . '/015-move-errors.xml';
file_put_contents($filename, $xmlstring);

// Load test data into a new XML Reader
$reader = new XMLReader();
if (!$reader->open($filename)) {
    exit('XML could not be read');
}

// Parse the data
while ($reader->read()) {
    if ($reader->nodeType != XMLREADER::END_ELEMENT) {
        // Find the book node
        if ($reader->nodeType == XMLREADER::ELEMENT && $reader->name == 'book') {
            // Test for missing namespace argument
            try {
                $reader->moveToAttributeNs('idx', null);
            } catch (ValueError $exception) {
                echo $exception->getMessage() . "\n";
            }
        }
    }
}

// clean up
$reader->close();
?>
--CLEAN--
<?php
unlink(__DIR__.'/015-move-errors.xml');
?>
--EXPECT--
XMLReader::moveToAttributeNs(): Argument #2 ($namespaceURI) cannot be empty
