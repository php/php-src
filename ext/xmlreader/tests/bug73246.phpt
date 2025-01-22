--TEST--
Bug #73246 (XMLReader: encoding length not checked)
--EXTENSIONS--
xmlreader
--FILE--
<?php
$reader = new XMLReader();
try {
    $reader->open(__FILE__, "UTF\0-8");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $reader->XML('<?xml version="1.0"?><root/>', "UTF\0-8");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    XMLReader::fromStream(fopen('php://memory', 'r'), encoding: "UTF\0-8");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
XMLReader::open(): Argument #2 ($encoding) must not contain any null bytes
XMLReader::XML(): Argument #2 ($encoding) must not contain any null bytes
XMLReader::fromStream(): Argument #2 ($encoding) must not contain any null bytes
