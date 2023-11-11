--TEST--
JIT ASSIGN_DIM: 005
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
set_error_handler(function ($code, $msg) {
	echo "Error: $msg\n";
    $GLOBALS['a'] = null;
});

$a[$c] =                                                                                                                                    
$a[$c] = 'x' ;
var_dump($a);
?>
--EXPECT--
Error: Undefined variable $c
Error: Undefined variable $c
NULL
