--TEST--
GH-16135: output buffer over-allocated for aligned chunk sizes
--FILE--
<?php
ob_start(null, 8191);
ob_start(null, 8192);
ob_start(null, 8193);
var_dump(array_map(fn ($s) => $s["buffer_size"], ob_get_status(true)));
?>
--EXPECT--
array(3) {
  [0]=>
  int(8192)
  [1]=>
  int(8192)
  [2]=>
  int(12288)
}
