--TEST--
DOMElement::className
--EXTENSIONS--
dom
--FILE--
<?php

class MyStringable {
    public function __toString(): string {
        throw new Exception("foo");
    }
}

$dom = new DOMDocument();
$dom->loadXML('<html/>');

var_dump($dom->documentElement->className);
$dom->documentElement->className = "hello & world<>";
var_dump($dom->documentElement->className);
$dom->documentElement->className = "";
var_dump($dom->documentElement->className);
$dom->documentElement->className = "é";
var_dump($dom->documentElement->className);
$dom->documentElement->className = "\0";
var_dump($dom->documentElement->className);
$dom->documentElement->className = 12345;
var_dump($dom->documentElement->className);
try {
    $dom->documentElement->className = new MyStringable();
} catch (Throwable $e) {
    echo "Error: ", $e->getMessage(), "\n";
}
var_dump($dom->documentElement->className);
echo $dom->saveXML();

?>
--EXPECT--
string(0) ""
string(15) "hello & world<>"
string(0) ""
string(2) "é"
string(0) ""
string(5) "12345"
Error: foo
string(5) "12345"
<?xml version="1.0"?>
<html class="12345"/>
