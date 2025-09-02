--TEST--
DOMElement::id
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
$dom->loadXML('<html><div/></html>');
$div = $dom->documentElement->firstChild;

var_dump($div->id);
$div->id = "hello & world<>";
var_dump($div->id);
$div->id = "";
var_dump($div->id);
$div->id = "é";
var_dump($div->id);
$div->id = "\0";
var_dump($div->id);
$div->id = 12345;
var_dump($div->id);
try {
    $div->id = new MyStringable();
} catch (Throwable $e) {
    echo "Error: ", $e->getMessage(), "\n";
}
var_dump($div->id);
echo $dom->saveXML();

var_dump($dom->getElementById("12345") === $div);

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
<html><div id="12345"/></html>
bool(true)
