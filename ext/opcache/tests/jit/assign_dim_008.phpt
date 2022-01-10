--TEST--
JIT ASSIGN_DIM: 008
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function(int $a) {
  $arr = $a[] = (y);
  $arr = y;
  $c = $y = $arr[] = y($c);
}
?>
DONE
--EXPECT--
DONE
