--TEST--
JIT FETCH_DIM_RW: 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
set_error_handler(function(y$y) {
});
$k=[];
$y[$k]++;
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot access offset of type array on array in %s:%d
Stack trace:
#0 {main}

Next TypeError: {closure:%s:%d}(): Argument #1 ($y) must be of type y, int given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(2, 'Undefined varia...', '%s', 5)
#1 {main}
  thrown in %s on line %d