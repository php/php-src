--TEST--
preload_user is required when euid is 0 under non-cli SAPIs
--INI--
--EXTENSIONS--
opcache
posix
--SKIPIF--
<?php
require dirname(__DIR__, 3) . '/sapi/fpm/tests/skipif.inc';
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (posix_geteuid() !== 0) die('skip Test needs root user');
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
    '-dopcache.log_verbosity_level=2',
];

$tester = new FPM\Tester($cfg, $code);
$tester->start($args);
var_dump($tester->getLogLines(1));
$tester->terminate();
$tester->close();

?>
Done
--EXPECTF--
array(1) {
  [0]=>
  string(%d) "%sFatal Error "opcache.preload" requires "opcache.preload_user" when running under uid 0
"
}
Done
--CLEAN--
<?php
require_once dirname(__DIR__, 3) . '/sapi/fpm/tests/tester.inc';
FPM\Tester::clean();
?>
