--TEST--
JIT FETCH_DIM_R: 005
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
$gens = [];
for ($i = 0; $i < 5; $i++) {
    $gens[] = $gens[-1];
}
var_dump($gens);
?>
--EXPECTF--
Warning: Undefined array key -1 in %sfetch_dim_r_005.php on line 4

Warning: Undefined array key -1 in %sfetch_dim_r_005.php on line 4

Warning: Undefined array key -1 in %sfetch_dim_r_005.php on line 4

Warning: Undefined array key -1 in %sfetch_dim_r_005.php on line 4

Warning: Undefined array key -1 in %sfetch_dim_r_005.php on line 4
array(5) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  NULL
}
