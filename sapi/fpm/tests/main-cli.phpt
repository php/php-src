--TEST--
FPM: runs as the CLI when invoked under a php name
--SKIPIF--
<?php
include "skipif.inc";
$php = \FPM\Tester::findExecutable();
if (preg_match("/'--(disable-cli|enable-cli=no)'/", (string) shell_exec("$php -n -i"))) {
    die("skip the CLI SAPI was not built");
}
?>
--FILE--
<?php

require_once "tester.inc";

$fpm = \FPM\Tester::findExecutable();
$dir = __DIR__ . '/main-cli';
@mkdir($dir);

foreach (['php', 'php8', 'php-cgi'] as $name) {
    @unlink("$dir/$name");
    symlink($fpm, "$dir/$name");
    echo "$name: ", trim(shell_exec(escapeshellarg("$dir/$name") . " -n -r 'echo PHP_SAPI;' 2>&1 | head -1")), "\n";
}

echo "php-fpm: ", trim(shell_exec(escapeshellarg($fpm) . " -n -r 'echo PHP_SAPI;' 2>&1 | head -1")), "\n";

?>
--CLEAN--
<?php
$dir = __DIR__ . '/main-cli';
foreach (['php', 'php8', 'php-cgi'] as $name) {
    @unlink("$dir/$name");
}
@rmdir($dir);
?>
--EXPECTF--
php: cli
php8: cli
php-cgi: Usage: php-cgi [%s
php-fpm: Usage: php-fpm [%s
