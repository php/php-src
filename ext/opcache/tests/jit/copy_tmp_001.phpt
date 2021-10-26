--TEST--
JIT COPY_TMP: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function test() {
    echo "";
}

$a = [];
$a[test()] ??= 1;
var_dump($a);
?>
--EXPECT--
array(1) {
  [""]=>
  int(1)
}
