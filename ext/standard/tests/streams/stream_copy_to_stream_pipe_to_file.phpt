--TEST--
stream_copy_to_stream() with a pipe as $source and file as $dest
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php

$descriptors = [1 => ['pipe', 'w']];
$proc = proc_open(
    [PHP_BINARY, '-n', '-r', 'echo str_repeat("p", 5000);'],
    $descriptors,
    $pipes
);
var_dump(is_resource($proc));

$source = $pipes[1];
$tmp = tmpfile();

$copied = stream_copy_to_stream($source, $tmp);
var_dump($copied);

fseek($tmp, 0, SEEK_SET);
$content = stream_get_contents($tmp);
var_dump(strlen($content));
var_dump($content === str_repeat("p", 5000));

fclose($tmp);
fclose($source);
proc_close($proc);
?>
--EXPECT--
bool(true)
int(5000)
int(5000)
bool(true)
