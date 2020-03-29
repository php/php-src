--TEST--
Bug #79423 (copy command is limited to size of file it can copy)
--SKIPIF--
<?php
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
?>
--FILE--
<?php
$orig = __DIR__ . '/bug79423_orig.txt';
$copy = __DIR__ . '/bug79423_copy.txt';

// create file of 0xFFFFFFFF bytes (SIZE_MAX on 32bit systems)
$text = str_repeat('*', 1024 * 1024);
$stream = fopen($orig, 'w');
for ($i = 0; $i < 4095; $i++) {
    fwrite($stream, $text);
}
fwrite($stream, $text, strlen($text) - 1);
fclose($stream);

var_dump(copy($orig, $copy));
var_dump(filesize($orig) === filesize($copy));
?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug79423_orig.txt');
unlink(__DIR__ . '/bug79423_copy.txt');
?>
--EXPECT--
bool(true)
bool(true)
