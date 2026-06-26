--TEST--
GH-21682 (ZipArchive subclass without overrides inherits the serialization throw)
--EXTENSIONS--
zip
--FILE--
<?php
class MyZip extends ZipArchive {}

$zip = new MyZip();
try {
    serialize($zip);
    echo "ERROR: should have thrown\n";
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:5:"MyZip":0:{}');
    echo "ERROR: should have thrown\n";
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Serialization of 'MyZip' is not allowed, override __serialize() and __unserialize() to implement it
Unserialization of 'MyZip' is not allowed, override __serialize() and __unserialize() to implement it
