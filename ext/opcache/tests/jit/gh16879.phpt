--TEST--
GH-16879 (JIT dead code skipping does not update call_level)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=32M
opcache.jit=1235
opcache.jit_hot_func=1
--FILE--
<?php
match(0){};
var_dump(new stdClass);
var_dump(3);
?>
--EXPECTF--
Fatal error: Uncaught UnhandledMatchError: Unhandled match case 0 in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
