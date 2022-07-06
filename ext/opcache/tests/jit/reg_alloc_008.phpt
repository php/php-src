--TEST--
Register Alloction 008: Incorrect deoptimization code
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function foo($a) {
    for($i=0; $i<1; $i++)
          $a=$y / $a = $a + $a != ($a);
}
foo(7);
?>
--EXPECTF--
Warning: Undefined variable $y in %sreg_alloc_008.php on line 4
