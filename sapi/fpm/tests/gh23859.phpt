--TEST--
FPM: GH-23859 (Map pointers grow after OPcache restarts in another worker)
--EXTENSIONS--
opcache
zend_test
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[worker]
listen = {{ADDR[worker]}}
pm = static
pm.max_children = 1
[reset]
listen = {{ADDR[reset]}}
pm = static
pm.max_children = 1
EOT;

$code = <<<'EOT'
<?php
class Test {
    public static function run() {
        static $i = 0;
        return ++$i;
    }
}
if (Test::run() !== 1) {
    throw new Exception('Static variables were not reset');
}
echo zend_get_map_ptr_last();
EOT;

$tester = new FPM\Tester($cfg, $code);
$resetFile = $tester->getPrefixedFile('reset.src.php');
file_put_contents($resetFile, <<<'EOT'
<?php
if (isset($_GET['reset'])) {
    var_dump(opcache_reset());
} else {
    echo opcache_get_status(false)['opcache_statistics']['manual_restarts'];
}
EOT);

$opcache = ini_get('extension_dir') . DIRECTORY_SEPARATOR . 'opcache.' . PHP_SHLIB_SUFFIX;
$extraArgs = is_file($opcache) ? ['-dzend_extension=' . $opcache] : [];
$tester->start($extraArgs, extensions: ['zend_test'], iniEntries: [
    'opcache.enable' => '1',
    'opcache.file_update_protection' => '0',
]);
$tester->expectLogStartNotices();
$tester->request(address: '{{ADDR[reset]}}', scriptFilename: $resetFile)->expectBody('0');
$before = $tester->request(address: '{{ADDR[worker]}}')->getBody();
if (!is_numeric($before)) {
    throw new Exception('Invalid map pointer count');
}
$tester->request(address: '{{ADDR[worker]}}')->expectBody($before);

/* Complete each restart in the other pool before the stale worker persists again. */
for ($i = 1; $i <= 3; $i++) {
    $tester->request(query: 'reset=1', address: '{{ADDR[reset]}}', scriptFilename: $resetFile)
        ->expectBody('bool(true)');
    $tester->request(address: '{{ADDR[reset]}}', scriptFilename: $resetFile)->expectBody((string) $i);
    $tester->request(address: '{{ADDR[worker]}}')->expectBody($before);
    $tester->request(address: '{{ADDR[worker]}}')->expectBody($before);
}

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
@unlink(__DIR__ . '/gh23859.reset.src.php');
?>
