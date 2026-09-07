--TEST--
Undef to exception for assign dim offset
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
set_error_handler(function($_, $m){
    throw new Exception($m);
});
function test1() {
    $a = [];
    $res = $a[$undef] = null;
}
function test2() {
    $a = [];
    $res = $a[$undef] += 1;
}
function test3() {
    $a = [];
    $res = isset($a[$undef]);
}
try {
    test1();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    test2();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Undefined variable $undef
Exception: Undefined variable $undef
