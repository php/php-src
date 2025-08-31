--TEST--
JIT ASSIGN_DIM: 006
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function foo($s) {
    $s = "123";
    for($i = 0; $i < 5; $i++) {
    	$x = $s[-5] = "x";
    }
	var_dump($x);
}
foo("123");
?>
--EXPECTF--
Warning: Illegal string offset -5 in %sassign_dim_006.php on line 5

Warning: Illegal string offset -5 in %sassign_dim_006.php on line 5

Warning: Illegal string offset -5 in %sassign_dim_006.php on line 5

Warning: Illegal string offset -5 in %sassign_dim_006.php on line 5

Warning: Illegal string offset -5 in %sassign_dim_006.php on line 5
NULL
