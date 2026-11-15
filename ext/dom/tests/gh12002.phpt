--TEST--
GH-12002 (DOMDocument::encoding memory leak with invalid encoding)
--EXTENSIONS--
dom
zend_test
--FILE--
<?php

$dom = new DOMDocument();
$dom->encoding = zend_return_non_interned_string('utf-8');
var_dump($dom->encoding);
try {
    $dom->encoding = zend_return_non_interned_string('foobar');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($dom->encoding);
$dom->encoding = zend_return_non_interned_string('utf-16le');
var_dump($dom->encoding);
try {
    $dom->encoding = NULL;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($dom->encoding);

?>
--EXPECT--
string(5) "utf-8"
ValueError: Invalid document encoding
string(5) "utf-8"
string(8) "utf-16le"
ValueError: Invalid document encoding
string(8) "utf-16le"
