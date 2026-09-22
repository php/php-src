--TEST--
Bug #73246 (XMLReader: encoding length not checked)
--EXTENSIONS--
xmlreader
--FILE--
<?php
$reader = new XMLReader();
try {
    $reader->open(__FILE__, "UTF\0-8");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $reader->XML('<?xml version="1.0"?><root/>', "UTF\0-8");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    XMLReader::fromStream(fopen('php://memory', 'r'), encoding: "UTF\0-8");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: XMLReader::open(): Argument #2 ($encoding) must not contain any null bytes
ValueError: XMLReader::XML(): Argument #2 ($encoding) must not contain any null bytes
ValueError: XMLReader::fromStream(): Argument #2 ($encoding) must not contain any null bytes
