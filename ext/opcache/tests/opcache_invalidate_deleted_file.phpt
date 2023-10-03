--TEST--
opcache_invalidate() should invalidate deleted file
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.validate_timestamps=0
--FILE--
<?php

$file = __DIR__ . DIRECTORY_SEPARATOR . pathinfo(__FILE__, PATHINFO_FILENAME) . '.inc';
file_put_contents($file, <<<PHP
<?php
return 42;
PHP);
var_dump(include $file);
unlink($file);
var_dump(include $file);
var_dump(opcache_invalidate($file));
var_dump(@(include $file));

?>
--EXPECT--
int(42)
int(42)
bool(true)
bool(false)
