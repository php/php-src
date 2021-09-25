--TEST--
Bug #81409: Incorrect JIT code for ADD with a reference to array
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=tracing
--FILE--
<?php
function foo(&$a) {
    $n = count($a);	
    $a = $a + [$n=>1];
}
function bar() {
    $x = [];
    for ($i = 0; $i < 200; $i++) {
        foo($x);
    }
    var_dump(count($x));
}
bar();
?>
--EXPECT--
int(200)