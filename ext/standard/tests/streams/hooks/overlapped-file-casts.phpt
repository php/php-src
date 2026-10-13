--TEST--
Windows overlapped files: the FILE* and descriptor casts are served from a synchronous reopen
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--ENV--
PHP_IO_OVERLAPPED_FILES=1
--FILE--
<?php
$dir = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'ovl' . getmypid();
mkdir($dir);
$data = str_repeat("0123456789", 100);
file_put_contents("$dir/source.txt", $data);

// CURLOPT_FILE: curl writes through the FILE* cast, the stream is a FILE* stream from then on
$out = fopen("$dir/curl.txt", 'wb');
$ch = curl_init('file:///' . str_replace('\\', '/', "$dir/source.txt"));
curl_setopt($ch, CURLOPT_FILE, $out);
var_dump(curl_exec($ch));
fwrite($out, "tail");
fclose($out);
var_dump(file_get_contents("$dir/curl.txt") === $data . "tail");

// proc_open() children: the descriptors they inherit are synchronous and at the stream's position
$in = fopen("$dir/source.txt", 'rb');
fread($in, 990);
$childOut = fopen("$dir/child.txt", 'wb');
$proc = proc_open([PHP_BINARY, '-n', '-r', 'echo strrev(stream_get_contents(STDIN));'], [0 => $in, 1 => $childOut], $pipes);
var_dump(proc_close($proc));
fclose($in);
fclose($childOut);
var_dump(file_get_contents("$dir/child.txt"));

// A select on a file: always ready
$f = fopen("$dir/source.txt", 'rb');
$r = [$f]; $w = $e = null;
var_dump(stream_select($r, $w, $e, 0), fread($f, 4));
fclose($f);

unlink("$dir/source.txt");
unlink("$dir/curl.txt");
unlink("$dir/child.txt");
rmdir($dir);
?>
--EXPECT--
bool(true)
bool(true)
int(0)
string(10) "9876543210"
int(1)
string(4) "0123"
