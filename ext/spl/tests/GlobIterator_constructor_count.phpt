--TEST--
GlobIterator without constructor breaks count()
--FILE--
<?php
$rc = new ReflectionClass(GlobIterator::class);
$in = $rc->newInstanceWithoutConstructor();
try {
    count($in);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: GlobIterator is not initialized
