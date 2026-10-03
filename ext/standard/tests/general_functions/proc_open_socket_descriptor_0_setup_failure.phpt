--TEST--
proc_open() closes descriptor 0 when setup fails after socket allocation
--SKIPIF--
<?php
if (!function_exists("proc_open")) {
    die("skip proc_open() unavailable");
}
if (!@is_dir("/proc/self/fd")) {
    die("skip requires /proc/self/fd");
}
?>
--FILE--
<?php
$code = <<<'PHP'
fclose(STDIN);
var_dump(@proc_open("true", [0 => ["socket"], 1 => ["bogus_type"]], $pipes));
$fd = fopen("/dev/null", "r");
var_dump(readlink("/proc/self/fd/0"));
fclose($fd);
PHP;
$process = proc_open([PHP_BINARY, "-n", "-r", $code], [1 => ["pipe", "w"]], $pipes);
echo stream_get_contents($pipes[1]);
fclose($pipes[1]);
var_dump(proc_close($process));
?>
--EXPECT--
bool(false)
string(9) "/dev/null"
int(0)
