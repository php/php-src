--TEST--
JIT ASSIGN_DIM: 005
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
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
array(1) {
  [""]=>
  string(1) "x"
}
Error: Undefined variable $c
Error: Using null as an array offset is deprecated, use an empty string instead
Error: Undefined variable $c
Error: Using null as an array offset is deprecated, use an empty string instead
