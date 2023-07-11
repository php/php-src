--TEST--
HOST/PATH ini sections test for cli
--SKIPIF--
<?php
if (!getenv("TEST_PHP_EXECUTABLE")) die("skip TEST_PHP_EXECUTABLE not set");
if (substr(PHP_OS, 0, 3) == "WIN") die("skip non windows test");
?>
--FILE--
<?php
$php = getenv("TEST_PHP_EXECUTABLE_ESCAPED");
$args = getenv("TEST_PHP_EXTRA_ARGS");
$cwd = getcwd();
$ini_file = __DIR__ . "/023.ini";
$ini_file_escaped = escapeshellarg($ini_file);
file_put_contents($ini_file, <<<INI
; no sections should match as cli doesn't support any
max_input_vars = 4
[PATH={$cwd}]
max_input_vars = 5
[PATH=/does/not/exist]
max_input_vars = 6
[HOST=some_fake_host]
max_input_vars = 7
INI
);
$desc = array(
    0 => array("pipe", "r"),
    1 => array("pipe", "w"),
    2 => array("pipe", "w"),
);
$pipes = array();
$proc = proc_open("$php $args -c $ini_file_escaped -r 'echo ini_get(\"max_input_vars\");'", $desc, $pipes);
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
--EXPECT--
string(1) "4"
string(0) ""
