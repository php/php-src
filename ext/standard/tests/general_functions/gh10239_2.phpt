--TEST--
GH-10239 (proc_close after proc_get_status always returns -1)
--SKIPIF--
<?php
if (PHP_OS != "Linux") die("skip, only for linux");
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--FILE--
<?php
$p = proc_open('false', array(), $foo);
usleep(2 * 1000 * 1000);
var_dump(proc_get_status($p));
var_dump(proc_get_status($p));
?>
--EXPECTF--
array(8) {
  ["command"]=>
  string(5) "false"
  ["pid"]=>
  int(%d)
  ["running"]=>
  bool(false)
  ["signaled"]=>
  bool(false)
  ["stopped"]=>
  bool(false)
  ["exitcode"]=>
  int(1)
  ["termsig"]=>
  int(0)
  ["stopsig"]=>
  int(0)
}
array(8) {
  ["command"]=>
  string(5) "false"
  ["pid"]=>
  int(%d)
  ["running"]=>
  bool(false)
  ["signaled"]=>
  bool(false)
  ["stopped"]=>
  bool(false)
  ["exitcode"]=>
  int(1)
  ["termsig"]=>
  int(0)
  ["stopsig"]=>
  int(0)
}
