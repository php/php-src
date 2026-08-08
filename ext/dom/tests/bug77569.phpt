--TEST--
Bug #77569 (Write Access Violation in DomImplementation)
--EXTENSIONS--
dom
--FILE--
<?php
$imp = new DOMImplementation;
$dom = $imp->createDocument("", "");
try {
    $dom->encoding = null;
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: Invalid document encoding
