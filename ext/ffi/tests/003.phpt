--TEST--
FFI 003: Forward tag/typedef declarations
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
var_dump($ffi->new("struct _a"));
var_dump($ffi->new("struct _b"));
var_dump($ffi->new("c"));
var_dump($ffi->new("d"));
try {
	var_dump($ffi->new("struct _e"));
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
FFI\ParserException: incomplete 'struct _e' at line 1
FFI\ParserException: incomplete 'struct _f' at line 1
ok
