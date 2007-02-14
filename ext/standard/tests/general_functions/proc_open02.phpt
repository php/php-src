--TEST--
proc_open
--SKIPIF--
<?php
if (!is_executable('/bin/sleep')) echo 'skip no sleep';
if (!is_executable('/bin/nohup')) echo 'skip no nohup';
?>
--FILE--
<?php
$ds = array(array('pipe', 'r'));

$cat = proc_open(
	'/bin/nohup /bin/sleep 50',
	$ds,
	$pipes
);

var_dump(proc_terminate($cat, 1)); // send a SIGHUP
sleep(1);
var_dump(proc_get_status($cat));

var_dump(proc_terminate($cat)); // now really quit it
sleep(1);
var_dump(proc_get_status($cat));

proc_close($cat);

echo "Done!\n";

?>
--EXPECTF--
bool(true)
array(8) {
  ["command"]=>
  string(24) "/bin/nohup /bin/sleep 50"
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
  string(24) "/bin/nohup /bin/sleep 50"
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
