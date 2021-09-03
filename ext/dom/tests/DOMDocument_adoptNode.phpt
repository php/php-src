--TEST--
DOMDocument::adoptNode not implemented
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML("<root />");

try {
    $dom->adoptNode($dom->documentElement);
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
Not yet implemented
