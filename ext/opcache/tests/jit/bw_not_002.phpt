--TEST--
JIT BW_NOT: 002 Exception handling
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
function test() {
	$j = 0;
    for ($i = 0; $i < 10; $i++) {
        $a = ~$j - $a = $j + $j = !$j = $j++;
    }
}
test();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot perform bitwise not on true in %sbw_not_002.php:5
Stack trace:
#0 %sbw_not_002.php(8): test()
#1 {main}
  thrown in %sbw_not_002.php on line 5
