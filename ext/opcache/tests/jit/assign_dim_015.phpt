--TEST--
JIT ASSIGN_DIM: 015
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
set_error_handler(function($code, $msg) use (&$my_var) {
	echo "Error: $msg\n";
    $my_var = 0;
});
$my_var[] = $y;
?>
DONE
--EXPECT--
Error: Undefined variable $y
DONE
