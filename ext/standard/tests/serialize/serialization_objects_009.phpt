--TEST--
Custom unserialization of classes with no custom unserializer.
--FILE--
<?php
$ser = 'C:1:"C":6:{dasdas}';

try {
    unserialize($ser);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

eval('class C {}');

try {
    unserialize($ser);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

echo "Done";
?>
--EXPECT--
Exception: Class __PHP_Incomplete_Class has no unserializer
Exception: Class C has no unserializer
Done
