--TEST--
GH-16535 (UAF when using document as a child)
--EXTENSIONS--
dom
--FILE--
<?php

$v2 = new DOMDocument("t");

$v2->loadHTML("t");
$v4 = $v2->createElement('foo');
try {
    $v4->appendChild($v2);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
$v2->loadHTML("oU");
echo $v2->saveXML();

?>
--EXPECT--
Hierarchy Request Error
<?xml version="1.0" standalone="yes"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">
<html><body><p>oU</p></body></html>
