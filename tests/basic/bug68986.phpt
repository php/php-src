--TEST--
Bug #68986 (pointer returned by php_stream_fopen_temporary_file not validated in memory.c)
--INI--
default_charset=UTF-8
--FILE--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
	die('skip.. only for unix');
}

$fp = fopen('php://temp', 'r+');
$data = implode('', array_fill(0, (1024 * 1024 * 2), 'A'));
var_dump(fwrite($fp, $data)); 
fclose($fp);

$tempDir = getenv("TMPDIR");
mkdir($tempDir . '/php68986');
system("chmod 444 " . $tempDir . '/php68986');
putenv("TMPDIR=" . $tempDir . '/php68986');

$fp = fopen('php://temp', 'r+');
$data = implode('', array_fill(0, (1024 * 1024 * 2), 'A'));
var_dump(fwrite($fp, $data)); 
fclose($fp);
rmdir($tempDir . '/php68986');

?>
--EXPECT--
int(2097152)
int(2088960)
