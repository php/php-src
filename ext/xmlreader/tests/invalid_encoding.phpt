--TEST--
Passing an invalid character encoding
--EXTENSIONS--
xmlreader
--FILE--
<?php
$reader = new XMLReader();
try {
    $reader->open(__FILE__, "does not exist");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$h = fopen("php://memory", "w+");
try {
    XMLReader::fromStream($h, encoding: "does not exist");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
fclose($h);

try {
    $reader->XML('<?xml version="1.0"?><root/>', "does not exist");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: XMLReader::open(): Argument #2 ($encoding) must be a valid character encoding
ValueError: XMLReader::fromStream(): Argument #2 ($encoding) must be a valid character encoding
ValueError: XMLReader::XML(): Argument #2 ($encoding) must be a valid character encoding
