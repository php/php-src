--TEST--
FFI 008: Array iteration
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$a = FFI::new("int[3]");
$a[1] = 10;
$a[2] = 20;
var_dump(count($a));
foreach ($a as $key => $val) {
    echo "$key => $val\n";
}

$a = FFI::new("struct {int x,y;}");
try {
    var_dump(count($a));
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}

try {
    foreach ($a as $key => $val) {
        echo "$key => $val\n";
    }
} catch (Throwable $e) {
    echo get_class($e) . ": " . $e->getMessage()."\n";
}
?>
--EXPECT--
int(3)
0 => 0
1 => 10
2 => 20
FFI\Exception: Attempt to count() on non C array
FFI\Exception: Attempt to iterate on non C array
