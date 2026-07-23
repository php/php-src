--TEST--
GH-21682 (ZipArchive serialization is rejected)
--EXTENSIONS--
zip
--FILE--
<?php
$a = new ZipArchive();
try {
    serialize($a);
    echo "ERROR: should have thrown\n";
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Serialization of 'ZipArchive' is not allowed, override __serialize() and __unserialize() to implement it
