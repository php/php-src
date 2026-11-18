--TEST--
clone of an object whose clone handler throws and returns NULL
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef();
$p = $ffi->new("int[2]", false);
FFI::free($p);
try {
    $q = clone $p;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "done", PHP_EOL;
?>
--EXPECT--
FFI\Exception: Use after free()
done
