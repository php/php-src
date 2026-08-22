--TEST--
SimpleXML: Attribute creation
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml =<<<EOF
<?xml version="1.0" encoding="ISO-8859-1" ?>
<foo/>
EOF;

$sxe = simplexml_load_string($xml);

try {
    $sxe[""] = "value";
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

$sxe["attr"] = "value";

echo $sxe->asXML();

$sxe["attr"] = "new value";

echo $sxe->asXML();

try {
    $sxe[] = "error";
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

__HALT_COMPILER();
?>
===DONE===
--EXPECT--
ValueError: Cannot create attribute with an empty name
<?xml version="1.0" encoding="ISO-8859-1"?>
<foo attr="value"/>
<?xml version="1.0" encoding="ISO-8859-1"?>
<foo attr="new value"/>
ValueError: Cannot append to an attribute list
