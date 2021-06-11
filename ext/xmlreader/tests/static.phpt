--TEST--
Calling XMLReader::open() and ::XML() statically
--EXTENSIONS--
xmlreader
--FILE--
<?php
$filename = __DIR__ . '/static.xml';

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books></books>';
file_put_contents($filename, $xmlstring);

$reader = XMLReader::open($filename);
while ($reader->read()) {
    echo $reader->name, "\n";
}

$reader = XMLReader::XML($xmlstring);
while ($reader->read()) {
    echo $reader->name, "\n";
}
?>
--EXPECT--
books
books
books
books
--CLEAN--
<?php
unlink(__DIR__ . '/static.xml');
?>
