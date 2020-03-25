--TEST--
Bug #74600 (crash (SIGSEGV) in _zend_hash_add_or_update_i)
--SKIPIF--
<?php
if (!getenv("TEST_PHP_EXECUTABLE")) die("skip TEST_PHP_EXECUTABLE not set");
if (substr(PHP_OS, 0, 3) == "WIN") die("skip non windows test");
?>
--FILE--
<?php
$php = getenv("TEST_PHP_EXECUTABLE");
$ini_file = __DIR__ . "/bug74600.ini";
file_put_contents($ini_file, <<<INI
[PHP]\n;\rs=\000\000=\n;\r[PATH\000]\000\376 =\n
INI
);
$desc = array(
    0 => array("pipe", "r"),
    1 => array("pipe", "w"),
    2 => array("pipe", "w"),
);
$pipes = array();
$proc = proc_open("$php -c $ini_file -r 'echo \"okey\";'", $desc, $pipes);
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
unlink(__DIR__ . "/bug74600.ini");
?>
--EXPECT--
string(4) "okey"
string(0) ""
