--TEST--
FETCH_STATIC_PROP_W should not return UNDEF
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
class F {
    static array $a;
}
F::$a[] = 2;
var_dump(F::$a);
?>
--EXPECT--
array(1) {
  [0]=>
  int(2)
}
