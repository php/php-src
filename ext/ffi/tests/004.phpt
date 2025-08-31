--TEST--
FFI 004: Enum declarations
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$ffi = FFI::cdef(<<<EOF
enum _a;
enum _a {e1};

enum _b {e2};
enum _b;

typedef enum _c c;
enum _c {e3};

enum _d {e4};
typedef enum _d d;

enum _e;

enum _f;
typedef enum _f f;

enum _g {
    _c1,
    _c2,
    _c3 = 1,
    _c4,
};
EOF
);
var_dump($ffi->new("enum _a"));
var_dump($ffi->new("enum _b"));
var_dump($ffi->new("c"));
var_dump($ffi->new("d"));
var_dump($ffi->new("int[_c2]"));
var_dump($ffi->new("int[_c3]"));
var_dump($ffi->new("int[_c4]"));
try {
    var_dump($ffi->new("enum _e"));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
try {
    var_dump($ffi->new("f"));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
echo "ok\n";
?>
--EXPECTF--
object(FFI\CData:enum _a)#%d (1) {
  ["cdata"]=>
  int(0)
}
object(FFI\CData:enum _b)#%d (1) {
  ["cdata"]=>
  int(0)
}
object(FFI\CData:enum _c)#%d (1) {
  ["cdata"]=>
  int(0)
}
object(FFI\CData:enum _d)#%d (1) {
  ["cdata"]=>
  int(0)
}
object(FFI\CData:int32_t[1])#%d (1) {
  [0]=>
  int(0)
}
object(FFI\CData:int32_t[1])#%d (1) {
  [0]=>
  int(0)
}
object(FFI\CData:int32_t[2])#%d (2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
FFI\ParserException: Incomplete enum "_e" at line 1
FFI\ParserException: Incomplete enum "_f" at line 1
ok
