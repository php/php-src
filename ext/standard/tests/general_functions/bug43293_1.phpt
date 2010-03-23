--TEST--
Bug #43293 (Multiple segfaults in getopt())
--INI--
register_argc_argv=Off
--FILE--
<?php
$argv = array(1, 2, 3); 
var_dump(getopt("abcd"));
var_dump($argv);
$argv = null;
var_dump(getopt("abcd"));
?>
--EXPECT--
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(false)

