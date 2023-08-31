--TEST--
Bug #46024 stream_select() doesn't return the correct number
--SKIPIF--
<?php
if (!getenv('TEST_PHP_EXECUTABLE_ESCAPED')) die("skip TEST_PHP_EXECUTABLE_ESCAPED not defined");
// Terminating the process may cause a bailout while writing out the phpinfo,
// which may leak a temporary hash table. This does not seems worth fixing.
if (getenv('SKIP_ASAN')) die("xleak Test may leak");
?>
--FILE--
<?php
$pipes = array();
$proc = proc_open(
    getenv('TEST_PHP_EXECUTABLE_ESCAPED') . " -n -i"
    ,array(0 => array('pipe', 'r'), 1 => array('pipe', 'w'))
    ,$pipes, __DIR__, array(), array()
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
object(Process)#1 (0) {
}
bool(true)
bool(true)
