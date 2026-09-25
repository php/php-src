--TEST--
Array-valued entries in PATH/HOST sections and .user.ini are ignored on activation
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php
include "include.inc";

$cgi = get_cgi_path();
$dir = __DIR__ . '/ini_section_array_value';
@mkdir($dir);
$script = "$dir/test.php";
$ini = "$dir/php.ini";

file_put_contents($script, '<?php var_dump(ini_get("memory_limit"), ini_get("precision"), ini_get("serialize_precision"));');
file_put_contents($ini, <<<INI
[PATH=$dir]
memory_limit = 17M
array_value[] = invalid

[HOST=example.test]
precision = 7
array_value[] = invalid
INI);
file_put_contents("$dir/.user.ini", <<<INI
serialize_precision = 5
array_value[] = invalid

[PATH=/nonexistent]
serialize_precision = 3
INI);

$env = getenv();
$env['REDIRECT_STATUS'] = '1';
$env['REQUEST_METHOD'] = 'GET';
$env['DOCUMENT_ROOT'] = $dir;
$env['SCRIPT_FILENAME'] = $script;
$env['PATH_TRANSLATED'] = $script;
$env['SERVER_NAME'] = 'example.test';

$process = proc_open([$cgi, '-q', '-c', $ini], [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes, $dir, $env);
echo stream_get_contents($pipes[1]);
var_dump(stream_get_contents($pipes[2]));
var_dump(proc_close($process));
?>
--CLEAN--
<?php
$dir = __DIR__ . '/ini_section_array_value';
@unlink("$dir/test.php");
@unlink("$dir/php.ini");
@unlink("$dir/.user.ini");
@rmdir($dir);
?>
--EXPECTF--
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

string(3) "17M"
string(1) "7"
string(1) "5"
string(0) ""
int(0)
