--TEST--
DOMDocument::$version write
--EXTENSIONS--
dom
--FILE--
<?php
class MyThrowingStringable {
    public function __toString(): string {
        throw new Exception("An exception was thrown");
    }
}

$dom = new DOMDocument;
var_dump($dom->version);
$dom->version = "foobar";
var_dump($dom->version);
echo $dom->saveXML();

try {
    $dom->version = new MyThrowingStringable;
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->version);
echo $dom->saveXML();
?>
--EXPECT--
string(3) "1.0"
string(6) "foobar"
<?xml version="foobar"?>
An exception was thrown
string(6) "foobar"
<?xml version="foobar"?>
