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
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

for ($sxe->rewind(); $sxe->valid(); $sxe->next()) {
    var_dump($sxe->key(), $sxe->current());
}

try {
    $sxe->current();
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
Error: Iterator not initialized or already consumed
string(4) "elem"
object(SimpleXMLElement)#3 (0) {
}
Error: Iterator not initialized or already consumed
