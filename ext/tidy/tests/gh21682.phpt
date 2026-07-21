--TEST--
GH-21682 (tidy and tidyNode should not be serializable)
--EXTENSIONS--
tidy
--FILE--
<?php
$t = new tidy();
try {
    serialize($t);
    echo "ERROR: should have thrown\n";
} catch (\Exception $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

$t->parseString("<html><body>test</body></html>");
$node = $t->body();
try {
    serialize($node);
    echo "ERROR: should have thrown\n";
} catch (\Exception $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Exception: Serialization of 'tidy' is not allowed
Exception: Serialization of 'tidyNode' is not allowed
