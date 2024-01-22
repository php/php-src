--TEST--
GH-12002 (DOMDocument::encoding memory leak with invalid encoding)
--EXTENSIONS--
dom
--FILE--
<?php

function make_nonconst(string $x) {
    // Defeat SCCP, even with inlining
    return str_repeat($x, random_int(1, 1));
}

$dom = new DOMDocument();
$dom->encoding = make_nonconst('utf-8');
var_dump($dom->encoding);
try {
    $dom->encoding = make_nonconst('foobar');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->encoding);
$dom->encoding = make_nonconst('utf-16le');
var_dump($dom->encoding);
try {
    $dom->encoding = NULL;
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->encoding);

?>
--EXPECT--
string(5) "utf-8"
Invalid document encoding
string(5) "utf-8"
string(8) "utf-16le"
Invalid document encoding
string(8) "utf-16le"
