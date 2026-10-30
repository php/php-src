--TEST--
GH-24139 (NULL pointer dereference in php_ini.c when expand_filepath() fails)
--SKIPIF--
<?php
include "skipif.inc";
if (PHP_OS_FAMILY === "Windows") die("skip not for Windows");
?>
--FILE--
<?php
$ini_file = __DIR__ . "/gh24139.ini";
file_put_contents($ini_file, "gh24139=ok\n");

$relative = str_repeat("./", intdiv(PHP_MAXPATHLEN - strlen(__DIR__), 2)) . "gh24139.ini";

$proc = proc_open(
    [getenv("TEST_PHP_EXECUTABLE"), "-c", $relative, "-r", 'var_dump(get_cfg_var("gh24139"));'],
    [1 => ["pipe", "w"]],
    $pipes,
    __DIR__
);
echo stream_get_contents($pipes[1]);
var_dump(proc_close($proc));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh24139.ini");
?>
--EXPECT--
bool(false)
int(0)
