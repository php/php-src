--TEST--
XMLReader: libxml2 XML Reader, Move cursor to an attribute, with invalid arguments
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--EXTENSIONS--
xmlreader
--FILE--
<?php
// Set up test data in a new file
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book num="1" idx="2">book1</book></books>';
$filename = __DIR__ . '/003-move-errors.xml';
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
            echo $reader->name . "\n";

            $attr = $reader->moveToNextAttribute();
            var_dump($attr);
            echo $reader->name . ": ";
            echo $reader->value . "\n";

            // Test for call with an empty string argument
            try {
                $reader->moveToAttribute('');
            } catch (ValueError $exception) {
                echo $exception->getMessage() . "\n";
            }

            // Ensure that node pointer has not changed position
            echo $reader->name . ": ";
            echo $reader->value . "\n";

            // Test for call by name for an attribute that doesn't exist
            $attr = $reader->moveToAttribute('isbn');
            var_dump($attr);
            // Ensure that node pointer has not changed position
            echo $reader->name . ": ";
            echo $reader->value . "\n";

            // Test for call by number for an attribute that doesn't exist
            $attr = $reader->moveToAttributeNo(911);
            var_dump($attr);
            // Oddly, node pointer moves back to the element in this case
            echo $reader->name . "\n";
        }
    }
}

// clean up
$reader->close();
?>
--CLEAN--
<?php
unlink(__DIR__.'/003-move-errors.xml');
?>
--EXPECT--
book
bool(true)
num: 1
XMLReader::moveToAttribute(): Argument #1 ($name) cannot be empty
num: 1
bool(false)
num: 1
bool(false)
book
