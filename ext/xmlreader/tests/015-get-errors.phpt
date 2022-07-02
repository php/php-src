--TEST--
XMLReader: libxml2 XML Reader, Move cursor to a named attribute within a namespace, with invalid arguments
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--EXTENSIONS--
xmlreader
--FILE--
<?php
// Set up test data in a new file
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books xmlns:ns1="http://www.ns1.namespace.org/" xmlns:ns2="http://www.ns2.namespace.org/"><book ns1:num="1" ns2:idx="2" ns1:idx="3" ns2:isbn="4">book1</book></books>';
$filename = __DIR__ . '/015-get-errors.xml';
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
            $attr = $reader->moveToNextAttribute();

            // Test for missing namespace argument
            try {
                $attr = $reader->getAttributeNs('idx', null);
            } catch (ValueError $exception) {
                echo $exception->getMessage() . "\n";
            }

            echo $reader->name . ": ";
            echo $reader->value . "\n";
        }
    }
}

// clean up
$reader->close();
?>
--CLEAN--
<?php
unlink(__DIR__.'/015-get-errors.xml');
?>
--EXPECTF--
Deprecated: XMLReader::getAttributeNs(): Passing null to parameter #2 ($namespace) of type string is deprecated in %s on line %d
XMLReader::getAttributeNs(): Argument #2 ($namespace) cannot be empty
ns1:num: 1
