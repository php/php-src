--TEST--
Using stream_resolve_include_path() on phar://
--INI--
phar.readonly=0
--FILE--
<?php

$file = __DIR__ . '/stream_resolve_include_path.phar';
$path = 'phar://' . $file . '/file.php';
file_put_contents($path, 'Test');
var_dump(stream_resolve_include_path($path) === $path);
unlink($file);

?>
--EXPECT--
bool(true)
