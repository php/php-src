--TEST--
JIT CAST: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test(?int $i) {
    $a = (array) $i;
    $a[-1] = 1;
    var_dump($a);
}
test(null);
?>
--EXPECT--
array(1) {
  [-1]=>
  int(1)
}
