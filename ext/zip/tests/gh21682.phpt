--TEST--
GH-21682 (ZipArchive is missing the NOT_SERIALIZABLE flag)
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
Serialization of 'ZipArchive' is not allowed
