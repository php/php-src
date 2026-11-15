--TEST--
Anonymous class names are not truncated in XMLReader error messages
--EXTENSIONS--
xmlreader
--FILE--
<?php

$reader = new class extends XMLReader {};
try {
    $reader->nodeType = 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Error: Cannot modify readonly property XMLReader@anonymous%0%s::$nodeType
