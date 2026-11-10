--TEST--
JIT FETCH_DIM_IS: string offsets given as strings behave like the VM
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
function test(string $s, string $k) {
    return $s[$k] ?? "default";
}

foreach (["1", "-1", "1 ", " 1", "1x", "-1x", "9x", "x", "", "1.5", "1.5x", "99999999999999999999"] as $k) {
    echo var_export($k, true), ": ";
    var_dump(test("abc", $k));
}
?>
--EXPECTF--
'1': string(1) "b"
'-1': string(1) "c"
'1 ': string(1) "b"
' 1': string(1) "b"
'1x': 
Warning: Illegal string offset "1x" in %s on line %d
string(1) "b"
'-1x': 
Warning: Illegal string offset "-1x" in %s on line %d
string(1) "c"
'9x': 
Warning: Illegal string offset "9x" in %s on line %d
string(7) "default"
'x': string(7) "default"
'': string(7) "default"
'1.5': string(7) "default"
'1.5x': string(7) "default"
'99999999999999999999': string(7) "default"
