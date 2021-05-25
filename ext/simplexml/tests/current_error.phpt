--TEST--
SimpleXML: invoking current() after the iterator has already been consumed
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml =<<<EOF
<?xml version='1.0'?>
<root>
<elem/>
</root>
EOF;

$sxe = simplexml_load_string($xml);

try {
    $sxe->current();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

foreach($sxe as $name => $data) {
    var_dump($name);
}

try {
    $sxe->current();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Iterator not initialized or already consumed
string(4) "elem"
Iterator not initialized or already consumed
