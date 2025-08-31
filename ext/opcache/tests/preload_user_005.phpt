--TEST--
preload_user=root is allowed under non-cli SAPIs
--INI--
--EXTENSIONS--
opcache
posix
--SKIPIF--
<?php
require dirname(__DIR__, 3) . '/sapi/fpm/tests/skipif.inc';
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (posix_geteuid() !== 0) die('skip Test needs root user');
if (posix_getpwnam('root') === false) die('skip Root user does not exist');
?>
--FILE--
<?php

require_once dirname(__DIR__, 3) . '/sapi/fpm/tests/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$code = <<<EOT
<?php
var_dump(function_exists("f1"));
var_dump(function_exists("f2"));
?>
OK
EOT;

$args = [];
if (file_exists(ini_get('extension_dir').'/opcache.so')) {
    $args[] = '-dzend_extension='.ini_get('extension_dir').'/opcache.so';
}
if (file_exists(ini_get('extension_dir').'/posix.so')) {
    $args[] = '-dextension='.ini_get('extension_dir').'/posix.so';
}
$args = [
    ...$args,
    '-dopcache.enable=1',
    '-dopcache.optimization_level=-1',
    '-dopcache.preload='.__DIR__.'/preload.inc',
    '-dopcache.preload_user=root',
    '-dopcache.log_verbosity_level=2',
    '-R',
];

$tester = new FPM\Tester($cfg, $code);
$tester->start($args);
$tester->expectLogStartNotices();
$tester
    ->request()
    ->expectBody([
        'bool(true)',
        'bool(false)',
        'OK',
    ]);
$tester->terminate();
$tester->close();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once dirname(__DIR__, 3) . '/sapi/fpm/tests/tester.inc';
FPM\Tester::clean();
?>
