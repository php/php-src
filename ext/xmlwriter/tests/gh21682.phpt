--TEST--
GH-21682 (XMLWriter should not be serializable)
--EXTENSIONS--
xmlwriter
--FILE--
<?php
$w = new XMLWriter();
try {
    serialize($w);
    echo "ERROR: should have thrown\n";
} catch (\Exception $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Exception: Serialization of 'XMLWriter' is not allowed
