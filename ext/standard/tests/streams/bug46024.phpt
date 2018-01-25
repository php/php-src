--TEST--
Bug #46024 stream_select() doesn't return the correct number
--SKIPIF--
<?php if (!getenv('TEST_PHP_EXECUTABLE')) die("skip TEST_PHP_EXECUTABLE not defined"); ?>
--FILE--
<?php
$php = realpath(getenv('TEST_PHP_EXECUTABLE'));
$pipes = array();
$proc = proc_open(
	"$php -n -i"
	,array(0 => array('pipe', 'r'), 1 => array('pipe', 'w'))
	,$pipes, dirname(__FILE__), array(), array('binary_pipes' => true)
);
var_dump($proc);
if (!$proc) {
	exit(1);
}
$r = array($pipes[1]);
$w = array($pipes[0]);
$e = null;
$ret = stream_select($r, $w, $e, 1);
var_dump($ret === (count($r) + count($w)));
fread($pipes[1], 1);

$r = array($pipes[1]);
$w = array($pipes[0]);
$e = null;
$ret = stream_select($r, $w, $e, 1);
var_dump($ret === (count($r) + count($w)));


foreach($pipes as $pipe) {
	fclose($pipe);
}
proc_terminate($proc);
if (defined('SIGKILL')) {
	proc_terminate($proc, SIGKILL);
} else {
	proc_terminate($proc);
}
proc_close($proc);
?>
--EXPECTF--
resource(%d) of type (process)
bool(true)
bool(true)
