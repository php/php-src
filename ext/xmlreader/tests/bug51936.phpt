--TEST--
Bug #51936 (Crash with clone XMLReader)
--EXTENSIONS--
xmlreader
--FILE--
<?php

$xmlreader = new XMLReader();
$xmlreader->xml("<a><b/></a>");

$xmlreader->next();

try {
    $xmlreader2 = clone $xmlreader;
    $xmlreader2->next();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class XMLReader
