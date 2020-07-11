--TEST--
realpath_cache_size() and realpath_cache_get()
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not on Windows');
}
?>
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
  %s(%f)
  ["is_dir"]=>
  bool(true)
  ["realpath"]=>
  string(%d) "%sfile"
  ["expires"]=>
  int(%d)
}
Done
