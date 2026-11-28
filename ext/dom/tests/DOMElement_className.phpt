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
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($dom->documentElement->className);
echo $dom->saveXML();

?>
--EXPECTF--
string(0) ""
string(15) "hello & world<>"
string(0) ""
string(2) "é"
string(0) ""
string(5) "12345"

Deprecated: Implicit conversion from MyStringable to string for property DOMElement::$className of type string is deprecated in %s on line %d
Exception: foo
string(5) "12345"
<?xml version="1.0"?>
<html class="12345"/>
