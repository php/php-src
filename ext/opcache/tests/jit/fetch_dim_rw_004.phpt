--TEST--
JIT FETCH_DIM_RW: 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
set_error_handler(function(y$y) {
});
$k=[];
$y[$k]++;
?>
--EXPECTF--
Fatal error: Uncaught TypeError: {closure}(): Argument #1 ($y) must be of type y, int given, called in %sfetch_dim_rw_004.php:2
Stack trace:
#0 %sfetch_dim_rw_004.php(5): {closure}(2, 'Undefined varia...', '%s', 5)
#1 {main}

Next TypeError: Illegal offset type in %sfetch_dim_rw_004.php:5
Stack trace:
#0 {main}
  thrown in %sfetch_dim_rw_004.php on line 5
