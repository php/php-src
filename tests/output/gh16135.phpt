--TEST--
GH-16135: output buffer over-allocated for aligned chunk sizes
--FILE--
<?php
ob_start(null, 0);
ob_start(null, 1);
ob_start(null, 2);
ob_start(null, 8191);
ob_start(null, 8192);
ob_start(null, 8193);
var_dump(array_map(fn ($s) => $s["buffer_size"], ob_get_status(true)));
?>
--EXPECT--
array(6) {
  [0]=>
  int(16384)
  [1]=>
  int(4096)
  [2]=>
  int(4096)
  [3]=>
  int(8192)
  [4]=>
  int(8192)
  [5]=>
  int(12288)
}
