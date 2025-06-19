--TEST--
Bug #78563: parsers should not be clonable
--EXTENSIONS--
xml
--FILE--
<?php

try {
    $parser = xml_parser_create();
    clone $parser;
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class XMLParser
