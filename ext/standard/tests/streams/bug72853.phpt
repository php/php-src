--TEST--
Bug #72853 (stream_set_blocking doesn't work)
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
    die('skip not for windows');
}
?>
--FILE--
<?php

$descs = array(
    0 => array('pipe', 'r'), // stdin
    1 => array('pipe', 'w'), // stdout
);

$p = proc_open("ls", $descs, $pipes, '.', NULL, NULL);

stream_set_blocking($pipes[1], false);
var_dump(stream_get_meta_data($pipes[1]));
stream_set_blocking($pipes[1], true);
while ($outs = fgets($pipes[1], 1024)) {
}
var_dump(stream_get_meta_data($pipes[1]));
proc_close($p);
?>
--EXPECT--
array(7) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(false)
  ["eof"]=>
  bool(false)
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
}
array(7) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(true)
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
}
