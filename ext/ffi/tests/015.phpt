--TEST--
FFI 015: Incomplete type usage
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
try {
    FFI::cdef("struct DIR; static struct DIR dir;");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct DIR; static struct DIR *ptr;");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct DIR; typedef struct DIR DIR; static DIR dir;");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct DIR; typedef struct DIR DIR; static DIR *ptr;");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct DIR; static struct DIR foo();");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct DIR; static struct DIR* foo();");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct DIR; static void foo(struct DIR);");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    FFI::cdef("struct DIR; static void foo(struct DIR*);");
    echo "ok\n";
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
ok
--EXPECT--
FFI\ParserException: Incomplete struct "DIR" at line 1
ok
FFI\ParserException: Incomplete struct "DIR" at line 1
ok
ok
ok
ok
ok
ok
