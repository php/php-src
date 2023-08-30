--TEST--
Cannot append to ArrayObject if backing value is an object
--EXTENSIONS--
spl
--FILE--
<?php
class MyClass {}
$c = new MyClass();

$ao = new ArrayObject($c);

try {
    $ao->append('no-key');
    var_dump($c);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $ao[] = 'no-key';
    var_dump($c);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Cannot append properties to objects, use ArrayObject::offsetSet() instead
Error: Cannot append properties to objects, use ArrayObject::offsetSet() instead
