--TEST--
PRE_INC/DEC numeric string
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function test($b) {
    $a = "0";
    $i = 0;
    while (is_numeric($a)) {
        $a .= $b;
        $a--;
        $i .= $a;
        $i++;
    }
    var_dump($a, $i);
}
test("0");
?>
--EXPECT--
string(5) "-INF0"
string(170) "0-2-12-112-1112-11112-111112-1111112-11111112-111111112-1111111112-11111111112-111111111112-1111111111112-11111111111112-1.1111111111111E+15-1.1111111111111E+141-ING-INF1"
