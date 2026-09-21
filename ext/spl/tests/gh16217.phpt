--TEST--
GH-16217 (SplFileObject methods on an uninitialized object segfault)
--FILE--
<?php
function uninitialized(): SplFileObject {
    return (new ReflectionClass(SplFileObject::class))->newInstanceWithoutConstructor();
}

try {
    (new ReflectionMethod(SplFileObject::class, "fputcsv"))->invoke(uninitialized(), []);
} catch (Error $e) {
    echo "fputcsv: ", $e->getMessage(), "\n";
}

try {
    (new ReflectionMethod(SplFileObject::class, "next"))->invoke(uninitialized());
} catch (Error $e) {
    echo "next: ", $e->getMessage(), "\n";
}

$obj = uninitialized();
(new ReflectionMethod(SplFileObject::class, "setFlags"))->invoke($obj, SplFileObject::READ_AHEAD);
try {
    (new ReflectionMethod(SplFileObject::class, "next"))->invoke($obj);
} catch (Error $e) {
    echo "next (READ_AHEAD): ", $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
fputcsv: Object not initialized
next: Object not initialized
next (READ_AHEAD): Object not initialized
Done
