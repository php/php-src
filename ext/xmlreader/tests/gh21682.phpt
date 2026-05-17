--TEST--
GH-21682 (XMLReader should not be serializable)
--EXTENSIONS--
xmlreader
--FILE--
<?php
$r = new XMLReader();
try {
    serialize($r);
    echo "ERROR: should have thrown\n";
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Serialization of 'XMLReader' is not allowed
