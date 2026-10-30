--TEST--
GH-12208 (SimpleXML infinite loop when a cast is used inside a foreach)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = "<root><a>1</a><a>2</a></root>";
$xml = simplexml_load_string($xml);

$a = $xml->a;

foreach ($a as $test) {
    var_dump((string) $a->current());
    var_dump((string) $a);
    var_dump((bool) $a);
}

?>
--EXPECT--
string(1) "1"
string(1) "1"
bool(true)
string(1) "2"
string(1) "1"
bool(true)
