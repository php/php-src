--TEST--
proc_open
--SKIPIF--
<?php
if (!is_executable('/bin/sleep')) echo 'skip no sleep';
if (getenv('SKIP_SLOW_TESTS')) echo 'skip slow test';
?>
--FILE--
<?php
$ds = array(array('pipe', 'r'));

$cat = proc_open(
    '/bin/sleep 2',
    $ds,
    $pipes
);

usleep(20000); // let the OS run the sleep process before sending the signal

var_dump(proc_terminate($cat, 0)); // status check
usleep(20000);
var_dump(proc_get_status($cat));

var_dump(proc_terminate($cat)); // now really quit it
usleep(20000);
var_dump(proc_get_status($cat));

proc_close($cat);

echo "Done!\n";

?>
--EXPECTF--
bool(true)
array(8) {
  ["command"]=>
  string(12) "/bin/sleep 2"
  ["pid"]=>
  int(%d)
  ["running"]=>
  bool(true)
  ["signaled"]=>
  bool(false)
  ["stopped"]=>
  bool(false)
  ["exitcode"]=>
  int(-1)
  ["termsig"]=>
  int(0)
  ["stopsig"]=>
  int(0)
}
bool(true)
array(8) {
  ["command"]=>
  string(12) "/bin/sleep 2"
  ["pid"]=>
  int(%d)
  ["running"]=>
  bool(false)
  ["signaled"]=>
  bool(true)
  ["stopped"]=>
  bool(false)
  ["exitcode"]=>
  int(-1)
  ["termsig"]=>
  int(15)
  ["stopsig"]=>
  int(0)
}
Done!
