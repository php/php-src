--TEST--
FFI 020: read-only
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
try {
    $p = FFI::new("struct {int x; const int y;}");
    $p->x = 1;
    $p->y = 1;
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $p = FFI::new("struct {const int x; int y;}");
    $p->y = 1;
    $p->x = 1;
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $p = FFI::new("const struct {int x; int y;}");
    $p->x = 1;
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $p = FFI::new("const int[10]");
    $p[1] = 1;
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $p = FFI::new("const int * [1]");
    $p[0] = null;
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $p = FFI::new("int * const [1]");
    $p[0] = null;
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    $f = FFI::cdef("typedef int * const t[1];");
    $p = $f->new("t");
    $p[0] = null;
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
ok
--EXPECT--
FFI\Exception: Attempt to assign read-only field 'y'
FFI\Exception: Attempt to assign read-only field 'x'
FFI\Exception: Attempt to assign read-only location
FFI\Exception: Attempt to assign read-only location
ok
FFI\Exception: Attempt to assign read-only location
FFI\Exception: Attempt to assign read-only location
ok
