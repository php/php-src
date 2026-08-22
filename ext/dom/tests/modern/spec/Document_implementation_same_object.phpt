--TEST--
Document::$implementation should return the same object
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();
var_dump($dom->implementation === $dom->implementation);
$implementation = $dom->implementation;
var_dump($dom->implementation === $implementation);

$dom2 = Dom\XMLDocument::createEmpty();
var_dump($dom2->implementation === $dom->implementation);

var_dump($implementation);

try {
    clone $implementation;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
object(Dom\Implementation)#2 (0) {
}
Error: Trying to clone an uncloneable object of class Dom\Implementation
