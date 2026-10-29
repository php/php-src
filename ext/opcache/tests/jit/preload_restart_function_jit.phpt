--TEST--
Preloaded functions JIT-compiled on script load must survive an OPcache restart
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
opcache.jit=disable
opcache.jit_buffer_size=16M
--CONFLICTS--
server
--FILE--
<?php
$dir = __DIR__ . '/preload_restart_function_jit';
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
function preloaded_str(string $x): string {
    return strrev($x) . strlen($x);
}
PHP);

file_put_contents($dir . '/call.php', <<<'PHP'
<?php
echo preloaded_sum(1000), " ", preloaded_str("hello"), "\n";
PHP);

file_put_contents($dir . '/reset.php', <<<'PHP'
<?php
var_dump(opcache_reset());
PHP);

/* Compiling a new script after the restart reuses the rewound JIT buffer */
$code = "<?php\n";
for ($i = 0; $i < 200; $i++) {
    $code .= "function f$i(int \$n): int { \$s = $i; for (\$k = 0; \$k < \$n; \$k++) { \$s = (\$s * 31 + \$k) % 1000003; } return \$s; }\n";
}
$code .= "echo f0(10) + f199(10), \"\\n\";\n";
file_put_contents($dir . '/new.php', $code);

include __DIR__ . '/../php_cli_server.inc';

$ini = '-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.jit=function -d opcache.jit_buffer_size=16M'
    . ' -d opcache.validate_timestamps=0 -d opcache.file_update_protection=0 -d opcache.preload_user=root'
    . ' -d opcache.preload=' . $dir . '/preload.php';
if (file_exists(ini_get('extension_dir') . '/opcache.' . PHP_SHLIB_SUFFIX)) {
    $ini .= ' -d zend_extension=opcache.' . PHP_SHLIB_SUFFIX;
}
php_cli_server_start($ini);

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/jit/preload_restart_function_jit/';
foreach (['call', 'reset', 'new', 'call', 'call'] as $script) {
    echo @file_get_contents($base . $script . '.php') ?: "no response\n";
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/preload_restart_function_jit';
foreach (['preload', 'call', 'reset', 'new'] as $script) {
    @unlink("$dir/$script.php");
}
@rmdir($dir);
?>
--EXPECT--
1502868 olleh5
bool(true)
1289143
1502868 olleh5
1502868 olleh5
