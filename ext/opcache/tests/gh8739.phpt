--TEST--
GH-8739: OPcache restart in a server with several workers
--DESCRIPTION--
A scheduled restart happens in the next request once no request uses the cache, and the cache
works after it. Under ZTS this checks the per-process reader count and the reader lock shared by
the worker processes: a reference that is never released would delay the restart forever.
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--CONFLICTS--
server
--FILE--
<?php
$dir = __DIR__ . '/gh8739';
@mkdir($dir);
file_put_contents("$dir/status.php", <<<'PHP'
<?php
$status = opcache_get_status(false);
echo $status['opcache_enabled'] ? 'enabled' : 'disabled',
    ', restarts: ', $status['opcache_statistics']['manual_restarts'],
    ', ', opcache_is_script_cached(__FILE__) ? 'cached' : 'not cached', "\n";
PHP);
file_put_contents("$dir/reset.php", "<?php\nvar_dump(opcache_reset());\n");

include __DIR__ . '/php_cli_server.inc';

/* elsewhere the workers outlive the server without PDEATHSIG, see php_cli_server_pdeathsig.phpt */
if (PHP_OS === 'Linux' || PHP_OS === 'FreeBSD') {
    putenv('PHP_CLI_SERVER_WORKERS=2');
}
$ini = '-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.file_update_protection=0';
$ext = ini_get('extension_dir');
if (file_exists($ext . DIRECTORY_SEPARATOR . (PHP_OS_FAMILY === 'Windows' ? 'php_opcache.dll' : 'opcache.so'))) {
    $ini .= ' -d extension_dir=' . $ext . ' -d zend_extension=opcache';
}
php_cli_server_start($ini);

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/gh8739/';
foreach (['status', 'reset', 'status', 'status'] as $script) {
    echo @file_get_contents($base . $script . '.php') ?: "no response\n";
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh8739';
@unlink("$dir/status.php");
@unlink("$dir/reset.php");
@rmdir($dir);
?>
--EXPECT--
enabled, restarts: 0, cached
bool(true)
enabled, restarts: 1, cached
enabled, restarts: 1, cached
