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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($dom->version);
echo $dom->saveXML();
?>
--EXPECTF--
string(3) "1.0"
string(6) "foobar"
<?xml version="foobar"?>

Deprecated: Implicit conversion from MyThrowingStringable to string for property DOMDocument::$version of type ?string is deprecated in %s on line %d
Exception: An exception was thrown
string(6) "foobar"
<?xml version="foobar"?>
