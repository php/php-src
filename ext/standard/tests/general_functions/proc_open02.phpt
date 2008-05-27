--TEST--
proc_open
--SKIPIF--
<?php
if (!is_executable('/bin/sleep')) echo 'skip no sleep';
if (!is_executable('/usr/bin/nohup')) echo 'skip no nohup';
?>
--FILE--
<?php
$ds = array(array('pipe', 'r'));

$cat = proc_open(
	'/usr/bin/nohup /bin/sleep 50',
	$ds,
	$pipes
);

sleep(1); // let the OS run the nohup process before sending the signal

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
  [u"command"]=>
  unicode(28) "/usr/bin/nohup /bin/sleep 50"
  [u"pid"]=>
  int(%d)
  [u"running"]=>
  bool(true)
  [u"signaled"]=>
  bool(false)
  [u"stopped"]=>
  bool(false)
  [u"exitcode"]=>
  int(-1)
  [u"termsig"]=>
  int(0)
  [u"stopsig"]=>
  int(0)
}
bool(true)
array(8) {
  [u"command"]=>
  unicode(28) "/usr/bin/nohup /bin/sleep 50"
  [u"pid"]=>
  int(%d)
  [u"running"]=>
  bool(false)
  [u"signaled"]=>
  bool(true)
  [u"stopped"]=>
  bool(false)
  [u"exitcode"]=>
  int(-1)
  [u"termsig"]=>
  int(15)
  [u"stopsig"]=>
  int(0)
}
Done!
