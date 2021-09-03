--TEST--
XMLReader: libxml2 XML Reader, string data
--EXTENSIONS--
xmlreader
--FILE--
<?php

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books></books>';

$reader = new XMLReader();
$reader->XML($xmlstring);

// Only go through
while ($reader->read()) {
    echo $reader->name."\n";
}
$xmlstring = '';
$reader = new XMLReader();

try {
    $reader->XML($xmlstring);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
books
books
XMLReader::XML(): Argument #1 ($source) cannot be empty
