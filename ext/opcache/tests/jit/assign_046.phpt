--TEST--
JIT ASSIGN: incorrect assumption about in-memeory zval type
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function test() {
    $a = null;
    for ($i = 0; $i < 6; $i++) {
        var_dump($a);
        $a = $a + $a = +$a = $b;
    }
}
test();
?>
--EXPECTF--
NULL

Warning: Undefined variable $b in %sassign_046.php on line 6
int(0)

Warning: Undefined variable $b in %sassign_046.php on line 6
int(0)

Warning: Undefined variable $b in %sassign_046.php on line 6
int(0)

Warning: Undefined variable $b in %sassign_046.php on line 6
int(0)

Warning: Undefined variable $b in %sassign_046.php on line 6
int(0)

Warning: Undefined variable $b in %sassign_046.php on line 6
