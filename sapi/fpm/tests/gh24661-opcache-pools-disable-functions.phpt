--TEST--
FPM: GH-24661 - Script cached by a pool with other disable_functions calls the wrong function
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
[unrestricted]
listen = {{ADDR[unrestricted]}}
pm = static
pm.max_children = 1
[restricted]
listen = {{ADDR[restricted]}}
pm = static
pm.max_children = 1
php_admin_value[disable_functions] = getmypid
EOT;

/* Disabling getmypid() moves var_export() in the function table. The try/catch
 * keeps the calls in f() from being specialized to ZEND_INIT_FCALL_OFFSET. */
$code = <<<'EOT'
<?php
set_exception_handler(function (Throwable $e) {
    echo $e->getMessage(), "\n";
});
function f() {
    try {
        return gettype(getmypid());
    } catch (Error $e) {
        return $e->getMessage();
    }
}
echo f(), "\n";
echo var_export(true, true), "\n";
echo gettype(getmypid()), "\n";
EOT;

$expected = [
    'unrestricted' => ['integer', 'true', 'integer'],
    'restricted' => [
        'Call to undefined function getmypid()',
        'true',
        'Call to undefined function getmypid()',
    ],
];

$opcache = ini_get('extension_dir') . DIRECTORY_SEPARATOR . 'opcache.' . PHP_SHLIB_SUFFIX;
$extraArgs = is_file($opcache) ? ['-dzend_extension=' . $opcache] : [];

foreach ([['unrestricted', 'restricted'], ['restricted', 'unrestricted']] as $pools) {
    $tester = new FPM\Tester($cfg, $code);
    $tester->start($extraArgs, iniEntries: ['opcache.file_update_protection' => '0']);
    $tester->expectLogStartNotices();
    foreach ([...$pools, ...$pools] as $pool) {
        $tester->request(address: "{{ADDR[$pool]}}")->expectBody($expected[$pool]);
    }
    $tester->terminate();
    $tester->expectLogTerminatingNotices();
    $tester->close();
}

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
