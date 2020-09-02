--TEST--
Testing xpath() with invalid XML
--SKIPIF--
<?php if (!extension_loaded('simplexml')) die('skip simplexml extension not loaded'); ?>
--FILE--
<?php
// gracefully recover from parsing of invalid XML; not available in PHP
const XML_PARSE_RECOVER = 1;

// we're not interested in checking concrete warnings regarding invalid XML
$xml = @simplexml_load_string("XXXXXXX^", 'SimpleXMLElement', XML_PARSE_RECOVER);

// $xml is supposed to hold a SimpleXMLElement, but not FALSE/NULL
try {
    var_dump($xml->xpath("BBBB"));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
SimpleXMLElement is not properly initialized
