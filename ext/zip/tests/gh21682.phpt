--TEST--
GH-21682 (ZipArchive must not be serializable)
--EXTENSIONS--
zip
--FILE--
<?php
try {
    serialize(new ZipArchive());
} catch (\Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Serialization of 'ZipArchive' is not allowed
