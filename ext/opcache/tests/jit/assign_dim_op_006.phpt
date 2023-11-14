--TEST--
JIT ASSIGN_DIM_OP 006: Clobbering array be user error handler
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
$my_var = null;
set_error_handler(function() use(&$my_var) {
    $my_var = 0;
});

$my_var[0000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000] .= "xyz";
var_dump($my_var);
?>
--EXPECT--
int(0)
