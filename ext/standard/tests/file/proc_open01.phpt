--TEST--
proc_open() regression test 1 (proc_open() leak)
--FILE--
<?php
$pipes = array(1, 2, 3);
$orig_pipes = $pipes;
$php = getenv('TEST_PHP_EXECUTABLE'); 
if ($php === false) {
	die("no php executable defined");
} 
$proc = proc_open(
	"$php -n",
	array(0 => array('pipe', 'r'), 1 => array('pipe', 'w')),
	$pipes, getcwd(), array(), array('binary_pipes' => true)
);
if ($proc === false) {
	print "something went wrong.\n";
}
var_dump($pipes);
stream_set_blocking($pipes[1], FALSE);
$test_string = b"yay!\n";
fwrite($pipes[0], $test_string);
fflush($pipes[0]);
fclose($pipes[0]);
$cnt = '';
$n=0;
for ($left = strlen($test_string); $left > 0;) { 
	if (++$n >1000) {
	  print "terminated after 1000 iterations\n";
	  break;
	}
	$read_fds = array($pipes[1]);
	$write_fds = NULL;
	$exp_fds = NULL;
	$retval = stream_select($read_fds, $write_fds, $exp_fds, 5);
	if ($retval === false) {
		print "select() failed\n";
		break;
	}
	if ($retval === 0) {
		print "timed out\n";
		break;
	}
	$buf = fread($pipes[1], 1024);
	$cnt .= $buf;
	$left -= strlen($buf);
}
var_dump($cnt);
fclose($pipes[1]);
proc_close($proc);
var_dump($orig_pipes);
?>
--EXPECTF--
array(2) {
  [0]=>
  resource(%d) of type (stream)
  [1]=>
  resource(%d) of type (stream)
}
%unicode|string%(5) "yay!
"
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
