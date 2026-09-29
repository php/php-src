--TEST--
FFI callback keeps the object of an array callable alive
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef("typedef int (*cb_t)(int); struct S { cb_t f; };");

class Callback {
    public int $factor = 21;

    public function multiply(int $x): int {
        return $this->factor * $x;
    }

    public function __destruct() {
        echo "Callback::__destruct\n";
    }
}

$s = $ffi->new("struct S");
$s->f = [new Callback(), 'multiply'];
var_dump(($s->f)(2));
echo "Done\n";
?>
--EXPECT--
int(42)
Done
Callback::__destruct
