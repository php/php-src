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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Invalid document encoding
