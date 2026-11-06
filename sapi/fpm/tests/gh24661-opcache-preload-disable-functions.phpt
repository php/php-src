--TEST--
FPM: GH-24661 - Preloaded script calls the wrong function in a pool with disable_functions
--EXTENSIONS--
opcache
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[restricted]
listen = {{ADDR}}
pm = static
pm.max_children = 1
php_admin_value[disable_functions] = getmypid
EOT;

/* The master preloads the functions before the pool disables getmypid(). */
$code = <<<'EOT'
<?php
set_exception_handler(function (Throwable $e) {
    echo $e->getMessage(), "\n";
});
echo preloaded_f(), "\n";
preloaded_g();
EOT;

$opcache = ini_get('extension_dir') . DIRECTORY_SEPARATOR . 'opcache.' . PHP_SHLIB_SUFFIX;
$extraArgs = is_file($opcache) ? ['-dzend_extension=' . $opcache] : [];
$iniEntries = ['opcache.preload' => __DIR__ . '/gh24661-preload.inc'];
if (getenv('TEST_FPM_RUN_AS_ROOT')) {
    $iniEntries['opcache.preload_user'] = 'root';
}

$tester = new FPM\Tester($cfg, $code);
$tester->start($extraArgs, iniEntries: $iniEntries);
$tester->expectLogStartNotices();
$expected = [
    'Call to undefined function getmypid()',
    'true',
    'Call to undefined function getmypid()',
];
$tester->request()->expectBody($expected);
$tester->request()->expectBody($expected);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
