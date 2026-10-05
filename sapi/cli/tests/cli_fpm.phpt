--TEST--
php --fpm runs the FPM SAPI
--SKIPIF--
<?php
include "skipif.inc";
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
if (!str_contains((string) shell_exec("$php --fpm -n -v 2>&1"), 'fpm-fcgi')) {
    die("skip PHP was not built with --enable-cli-fpm");
}
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

// As the first argument, --fpm switches to the FPM SAPI.
preg_match('/^Server API => .*$/m', shell_exec("$php --fpm -n -i"), $m);
var_dump($m[0]);

// Anywhere else it is an ordinary argument.
var_dump(shell_exec("$php -n -r 'echo \$argv[1];' -- --fpm"));

// Running under a name starting with "php-fpm" also switches to the FPM SAPI.
$dir = __DIR__ . '/cli_fpm';
@mkdir($dir);
symlink(realpath(getenv('TEST_PHP_EXECUTABLE')), "$dir/php-fpm8");
preg_match('/^Server API => .*$/m', shell_exec(escapeshellarg("$dir/php-fpm8") . " -n -i"), $m);
var_dump($m[0]);
?>
--CLEAN--
<?php
$dir = __DIR__ . '/cli_fpm';
@unlink("$dir/php-fpm8");
@rmdir($dir);
?>
--EXPECT--
string(25) "Server API => FPM/FastCGI"
string(5) "--fpm"
string(25) "Server API => FPM/FastCGI"
