--TEST--
JIT FETCH_DIM_R: 012
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function foo() {
	$a = 0; $a1 = []; $a2 = [];
    for($i = 0; $i < 6; $i++) {
        $a1[] = &$y;
        $a2["$a1[$a] "] = $a += $y;
    }
    var_dump($a1, $a2);
}
foo();
?>
--EXPECT--
array(6) {
  [0]=>
  &NULL
  [1]=>
  &NULL
  [2]=>
  &NULL
  [3]=>
  &NULL
  [4]=>
  &NULL
  [5]=>
  &NULL
}
array(1) {
  [" "]=>
  int(0)
}
