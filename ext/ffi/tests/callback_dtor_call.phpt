--TEST--
FFI callback bound to a __call() trampoline
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef("typedef int (*cb_t)(int); struct S { cb_t f; cb_t g; };");

class Callback {
    public function __call(string $name, array $arguments): int {
        echo $name, "(", $arguments[0], ")\n";

        return $arguments[0] * 2;
    }
}

$callback = new Callback();
$s = $ffi->new("struct S");
$s->g = [$callback, 'unused'];
$s->f = [$callback, 'double'];
var_dump(($s->f)(1));
var_dump(($s->f)(2));
var_dump(($s->f)(3));
?>
--EXPECT--
double(1)
int(2)
double(2)
int(4)
double(3)
int(6)
