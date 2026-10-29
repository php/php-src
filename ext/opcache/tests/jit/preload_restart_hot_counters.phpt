--TEST--
Preloaded functions JIT-compiled by hot counters must be JIT-compiled again after an OPcache restart
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (ini_get('opcache.jit') === false) die('skip PHP is compiled without JIT');
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1235
opcache.jit_buffer_size=16M
--CONFLICTS--
server
--FILE--
<?php
$dir = __DIR__ . '/preload_restart_hot_counters';
@mkdir($dir);

file_put_contents($dir . '/preload.php', <<<'PHP'
<?php
function preloaded_sum(int $n): int {
    $s = 0;
    for ($i = 0; $i < $n; $i++) {
        $s += ($i * 3) ^ ($i >> 2);
    }
    return $s;
}
PHP);

/* A dynamic call keeps the function out of the call graph of the calling script */
file_put_contents($dir . '/call.php', <<<'PHP'
<?php
$f = 'preloaded_sum';
echo $f(1000), "\n";
PHP);

file_put_contents($dir . '/reset.php', <<<'PHP'
<?php
var_dump(opcache_get_status()['jit']['on']);
var_dump(opcache_reset());
PHP);

include __DIR__ . '/../php_cli_server.inc';

$ini = '-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.jit=1235 -d opcache.jit_buffer_size=16M'
    . ' -d opcache.jit_hot_func=1 -d opcache.validate_timestamps=0 -d opcache.file_update_protection=0'
    . ' -d opcache.preload_user=root -d opcache.preload=' . $dir . '/preload.php';
$opcache = ini_get('extension_dir') . '/opcache.' . PHP_SHLIB_SUFFIX;
if (file_exists($opcache)) {
    $ini .= ' -d zend_extension=' . $opcache;
}
php_cli_server_start($ini);

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/jit/preload_restart_hot_counters/';
$context = stream_context_create(['http' => ['timeout' => 5]]);
foreach (['call', 'reset', 'call', 'call'] as $script) {
    echo @file_get_contents($base . $script . '.php', false, $context) ?: "no response\n";
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/preload_restart_hot_counters';
foreach (['preload', 'call', 'reset'] as $script) {
    @unlink("$dir/$script.php");
}
@rmdir($dir);
?>
--EXPECT--
1502868
bool(true)
bool(true)
1502868
1502868
