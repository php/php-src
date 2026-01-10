--TEST--
JIT ASSIGN: incorrect type store elimination
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
opcache.optimization_level=0x7FFEBFFF
--FILE--
<?php
function test(){
    $j = 0;
    for($i=0; $i<10; $i++) {
        +$b = +$b = unserialize('');
        $y[] = 4;
        $a + ~$b = $j++;
     }
}
test();
?>
DONE
--EXPECTF--
Warning: Undefined variable $a in %sassign_048.php on line 7

Warning: Undefined variable $a in %sassign_048.php on line 7

Warning: Undefined variable $a in %sassign_048.php on line 7

Warning: Undefined variable $a in %sassign_048.php on line 7

Warning: Undefined variable $a in %sassign_048.php on line 7

Warning: Undefined variable $a in %sassign_048.php on line 7

Warning: Undefined variable $a in %sassign_048.php on line 7

Warning: Undefined variable $a in %sassign_048.php on line 7

Warning: Undefined variable $a in %sassign_048.php on line 7

Warning: Undefined variable $a in %sassign_048.php on line 7
DONE
