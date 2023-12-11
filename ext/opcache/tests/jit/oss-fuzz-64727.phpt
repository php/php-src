--TEST--
oss-fuzz #64727
--INI--
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
function test(){
    $a = null;
    $b = null;
    for($i = 0; $i < 2; $i++){
        $a = $a + $b;
        var_dump($a);
        $a = @[3][$a];
        var_dump($a);
    }
}
test();
?>
--EXPECT--
int(0)
int(3)
int(3)
NULL
