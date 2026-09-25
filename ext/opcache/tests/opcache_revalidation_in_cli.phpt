--TEST--
opcache revalidation should work properly in CLI mode
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.validate_timestamps=1
opcache.revalidate_freq=1
--FILE--
<?php
$file = __DIR__ . DIRECTORY_SEPARATOR . pathinfo(__FILE__, PATHINFO_FILENAME) . '.inc';

file_put_contents($file, <<<PHP
<?php
return 42;
PHP);

var_dump(opcache_invalidate($file, true));

var_dump(include $file);

file_put_contents($file, <<<PHP
<?php
return 1234;
PHP);

var_dump(include $file);

sleep(2);
touch($file);

var_dump(include $file);

@unlink($file);

?>
--EXPECT--
bool(true)
int(42)
int(42)
int(1234)
