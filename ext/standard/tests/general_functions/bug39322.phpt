--TEST--
Bug #39322 (proc_terminate() losing process resource)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (!is_executable('/bin/sleep')) echo 'skip sleep not found';
?>
--FILE--
<?php
$descriptors = array(
    0 => array('pipe', 'r'),
    1 => array('pipe', 'w'),
    2 => array('pipe', 'w'));

$pipes = array();

$process = proc_open('/bin/sleep 120', $descriptors, $pipes);

proc_terminate($process, 9);
sleep(1); // wait a bit to let the process finish
var_dump(proc_get_status($process));

echo "Done!\n";

?>
--EXPECTF--
array(9) {
  ["command"]=>
  string(14) "/bin/sleep 120"
  ["pid"]=>
  int(%d)
  ["cached"]=>
  bool(false)
  ["running"]=>
  bool(false)
  ["signaled"]=>
  bool(true)
  ["stopped"]=>
  bool(false)
  ["exitcode"]=>
  int(-1)
  ["termsig"]=>
  int(9)
  ["stopsig"]=>
  int(0)
}
Done!
