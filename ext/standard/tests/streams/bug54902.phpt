--TEST--
Bug #54902 (fseek inconsistencies with large (>2GB) files)
--SKIPIF--
<?php
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
if (disk_free_space(__DIR__) < 0xc0000000) die('skip not enough disk space');
?>
--FILE--
<?php
$data = str_repeat('ab', 4096);
$stream = fopen(__DIR__ . '/bug54902.txt', 'w+');
for ($i = 0; $i < 0x40001; ++$i) {
    fwrite($stream, $data);
}
rewind($stream);

var_dump(fseek($stream, 0x7fffffff, SEEK_SET));
var_dump(ftell($stream));
var_dump(fseek($stream, 1, SEEK_CUR));
var_dump(ftell($stream));
var_dump(fread($stream, 1));
var_dump(ftell($stream));
?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug54902.txt');
?>
--EXPECT--
int(0)
int(2147483647)
int(-1)
int(2147483647)
string(0) ""
int(2147483647)
