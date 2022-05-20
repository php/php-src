--TEST--
FFI 003: Forward tag/typedef declarations
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef(<<<EOF
struct _a;
struct _a {int x;};

struct _b {int x;};
struct _b;

typedef struct _c c;
struct _c {int x;};

struct _d {int x;};
typedef struct _d d;

struct _e;

struct _f;
typedef struct _f f;
EOF
);
var_dump(FFI::new("struct _a", cdef : $ffi));
var_dump(FFI::new("struct _b", cdef : $ffi));
var_dump(FFI::new("c", cdef : $ffi));
var_dump(FFI::new("d", cdef : $ffi));
try {
    var_dump(FFI::new("struct _e", cdef : $ffi));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    var_dump(FFI::new("f", cdef : $ffi));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
echo "ok\n";
?>
--EXPECTF--
object(FFI\CData:struct _a)#%d (1) {
  ["x"]=>
  int(0)
}
object(FFI\CData:struct _b)#%d (1) {
  ["x"]=>
  int(0)
}
object(FFI\CData:struct _c)#%d (1) {
  ["x"]=>
  int(0)
}
object(FFI\CData:struct _d)#%d (1) {
  ["x"]=>
  int(0)
}
FFI\ParserException: Incomplete struct "_e" at line 1
FFI\ParserException: Incomplete struct "_f" at line 1
ok
