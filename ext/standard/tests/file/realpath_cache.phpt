--TEST--
realpath_cache_size() and realpath_cache_get()
--FILE--
<?php

var_dump(realpath_cache_size());
$data = realpath_cache_get();
var_dump($data[__DIR__]);

echo "Done\n";
?>
--EXPECTF--	
int(%d)
array(4) {
  ["key"]=>
  int(%d)
  ["is_dir"]=>
  bool(true)
  ["realpath"]=>
  string(%d) "%sfile"
  ["expires"]=>
  int(%d)
}
Done
