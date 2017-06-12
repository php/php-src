--TEST--
HOST/PATH ini sections test for cli
--SKIPIF--
<?php
if (!getenv("TEST_PHP_EXECUTABLE")) die("skip TEST_PHP_EXECUTABLE not set");
if (substr(PHP_OS, 0, 3) == "WIN") die("skip non windows test");
?>
--FILE--
<?php
$php = getenv("TEST_PHP_EXECUTABLE");
$cwd = getcwd();
$ini_file = __DIR__ . "/023.ini";
file_put_contents($ini_file, <<<INI
; no sections should match as cli doesn't support any
memory_limit = 40M
[PATH={$cwd}]
memory_limit = 50M
[PATH=/does/not/exist]
memory_limit = 60M
[HOST=some_fake_host]
memory_limit = 70M
INI
);
$desc = array(
	0 => array("pipe", "r"),
	1 => array("pipe", "w"),
	2 => array("pipe", "w"),
);
$pipes = array();
$proc = proc_open("$php -c $ini_file -r 'echo ini_get(\"memory_limit\");'", $desc, $pipes);
if (!$proc) {
	exit(1);
}
var_dump(stream_get_contents($pipes[1]));
var_dump(stream_get_contents($pipes[2]));

proc_terminate($proc);
proc_close($proc);
?>
--CLEAN--
<?php
unlink(__DIR__ . "/023.ini");
?>
--EXPECTF--
string(3) "40M"
string(0) ""
