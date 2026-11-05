--TEST--
GH-25130: Harmonize opcache is_phar_file marker check with ext/phar
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php
$dir = __DIR__ . '/gh25130';
@mkdir($dir . '/dir.phar', 0777, true);
@mkdir($dir . '/dir.pharabcd', 0777, true);

file_put_contents($dir . '/prepend.php', <<<'PHP'
<?php
class Interceptor {
    public function url_stat(string $path, int $flags): array|false {
        $logFile = __DIR__ . '/stat_calls.log';
        file_put_contents($logFile, basename($path) . "\n", FILE_APPEND);
        return false;
    }
}
stream_wrapper_unregister('phar');
stream_wrapper_register('phar', 'Interceptor');
PHP);

file_put_contents($dir . '/dir.phar/test.php', '<?php echo 1;');
file_put_contents($dir . '/dir.pharabcd/test.php', '<?php echo 2;');
file_put_contents($dir . '/test.pharma.php', '<?php echo 3;');
file_put_contents($dir . '/test.pharma.report.php', '<?php echo 4;');
file_put_contents($dir . '/test.pharma.phar.php', '<?php echo 5;');
file_put_contents($dir . '/test.phar.php', '<?php echo 6;');
file_put_contents($dir . '/include_test_phar.php', '<?php include __DIR__ . "/test.phar";');
file_put_contents($dir . '/test.phar', '<?php echo 7;');

include 'php_cli_server.inc';
$ini = trim((string) getenv('TEST_PHP_EXTRA_ARGS'));
$ini .= ($ini !== '' ? ' ' : '') . '-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.file_update_protection=0 -d auto_prepend_file=' . $dir . '/prepend.php';
php_cli_server_start($ini);

$endpoints = [
    '/gh25130/dir.phar/test.php',
    '/gh25130/dir.pharabcd/test.php',
    '/gh25130/test.pharma.php',
    '/gh25130/test.pharma.report.php',
    '/gh25130/test.pharma.phar.php',
    '/gh25130/test.phar.php',
    '/gh25130/include_test_phar.php',
];

foreach ($endpoints as $endpoint) {
    file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . $endpoint);
    file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . $endpoint);
}

$logFile = $dir . '/stat_calls.log';
if (file_exists($logFile)) {
    echo file_get_contents($logFile);
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh25130';
@unlink($dir . '/dir.phar/test.php');
@unlink($dir . '/dir.pharabcd/test.php');
@unlink($dir . '/test.pharma.php');
@unlink($dir . '/test.pharma.report.php');
@unlink($dir . '/test.pharma.phar.php');
@unlink($dir . '/test.phar.php');
@unlink($dir . '/include_test_phar.php');
@unlink($dir . '/test.phar');
@unlink($dir . '/prepend.php');
@unlink($dir . '/stat_calls.log');
@rmdir($dir . '/dir.phar');
@rmdir($dir . '/dir.pharabcd');
@rmdir($dir);
?>
--EXPECT--
test.pharma.phar.php
test.phar.php
test.phar
