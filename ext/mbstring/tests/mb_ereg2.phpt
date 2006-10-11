--TEST--
mb_ereg() returning matches
--SKIPIF--
<?php if (!function_exists("mb_ereg")) print "skip"; ?>
--FILE--
<?php

$a = -1; $b = -1; $c = -1; 
mbereg($a, $b, $c); 
var_dump($a, $b, $c); 

mberegi($a, $b, $c); 
var_dump($a, $b, $c); 

mbereg_search_init($a, $b, $c); 
var_dump($a, $b, $c);

echo "Done\n";
?>
--EXPECTF--	
int(-1)
int(-1)
array(1) {
  [0]=>
  string(2) "-1"
}
int(-1)
int(-1)
array(1) {
  [0]=>
  string(2) "-1"
}

Notice: Array to string conversion in %s on line %d
int(-1)
int(-1)
array(1) {
  [0]=>
  string(2) "-1"
}
Done
