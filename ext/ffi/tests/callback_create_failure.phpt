--TEST--
FFI callback creation failure releases a __call trampoline
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef("struct E {}; typedef int (*cb_t)(struct E); struct S { cb_t f; };");

class Callback {
    public function __call(string $name, array $arguments): int {
        return 0;
    }
}

$s = $ffi->new("struct S");
try {
    $s->f = [new Callback(), 'compare'];
} catch (FFI\Exception $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
echo "Done\n";
?>
--EXPECT--
FFI\Exception: Cannot prepare callback CIF
Done
