--TEST--
UserCache\Cache: apache2handler partitions cache data by virtual host
--EXTENSIONS--
user_cache
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
$httpd = getenv('TEST_PHP_APACHE2HANDLER_HTTPD');
$module = getenv('TEST_PHP_APACHE2HANDLER_MODULE');
if ($httpd === false || $module === false) {
    die('skip set TEST_PHP_APACHE2HANDLER_HTTPD and TEST_PHP_APACHE2HANDLER_MODULE to run apache2handler boundary test');
}
if (!is_file($httpd) || !is_executable($httpd)) die('skip TEST_PHP_APACHE2HANDLER_HTTPD is not executable');
if (!is_file($module)) die('skip TEST_PHP_APACHE2HANDLER_MODULE is not a file');
?>
--FILE--
<?php

function user_cache_apache_free_port(): int
{
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    if ($server === false) {
        throw new RuntimeException($errstr);
    }

    $name = stream_socket_get_name($server, false);
    fclose($server);

    return (int) substr(strrchr($name, ':'), 1);
}

function user_cache_apache_rm_rf(string $path): void
{
    if (!file_exists($path)) {
        return;
    }

    if (!is_dir($path) || is_link($path)) {
        unlink($path);
        return;
    }

    foreach (scandir($path) as $entry) {
        if ($entry === '.' || $entry === '..') {
            continue;
        }
        user_cache_apache_rm_rf($path . DIRECTORY_SEPARATOR . $entry);
    }

    rmdir($path);
}

function user_cache_apache_request(int $port, string $host, string $path): string
{
    $fp = @stream_socket_client("tcp://127.0.0.1:$port", $errno, $errstr, 2);
    if ($fp === false) {
        throw new RuntimeException($errstr);
    }
    stream_set_timeout($fp, 5);

    fwrite($fp, "GET $path HTTP/1.1\r\nHost: $host\r\nConnection: close\r\n\r\n");
    $response = stream_get_contents($fp);
    fclose($fp);

    [$headers, $body] = explode("\r\n\r\n", $response, 2) + ['', ''];
    if (!str_starts_with($headers, 'HTTP/1.1 200') && !str_starts_with($headers, 'HTTP/1.0 200')) {
        throw new RuntimeException($headers . "\n" . $body);
    }

    return trim($body);
}

function user_cache_apache_wait($process, array $pipes, int $port): void
{
    for ($i = 0; $i < 50; $i++) {
        $status = proc_get_status($process);
        if (!$status['running']) {
            throw new RuntimeException(stream_get_contents($pipes[2]));
        }

        try {
            user_cache_apache_request($port, 'alpha.local', '/index.php?action=fetch');
            return;
        } catch (Throwable) {
            usleep(100000);
        }
    }

    throw new RuntimeException(stream_get_contents($pipes[2]));
}

$root = sys_get_temp_dir() . '/php-user-cache-apache-' . getmypid();
$alphaRoot = $root . '/alpha';
$betaRoot = $root . '/beta';
$process = null;
$pipes = [];

user_cache_apache_rm_rf($root);
mkdir($alphaRoot, 0777, true);
mkdir($betaRoot, 0777, true);

$script = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('default');
$key = 'apache2handler-boundary-key';
$action = $_GET['action'] ?? 'fetch';
$host = $_SERVER['SERVER_NAME'] ?? $_SERVER['HTTP_HOST'] ?? 'unknown';

if ($action === 'seed') {
    $cache->store($key, $host . '-value');
}

echo $host, ':', $cache->fetch($key, 'MISS'), ':', $cache->getPoolStatus()->getPoolName(), "\n";
PHP;

file_put_contents($alphaRoot . '/index.php', $script);
file_put_contents($betaRoot . '/index.php', $script);
file_put_contents($root . '/php.ini', implode("\n", [
    'user_cache.enable=1',
    'user_cache.shm_size=32M',
    'opcache.file_update_protection=0',
]));

$port = user_cache_apache_free_port();
$httpd = getenv('TEST_PHP_APACHE2HANDLER_HTTPD');
$module = getenv('TEST_PHP_APACHE2HANDLER_MODULE');
$moduleName = getenv('TEST_PHP_APACHE2HANDLER_MODULE_NAME') ?: 'php_module';
$extraConfig = getenv('TEST_PHP_APACHE2HANDLER_EXTRA_CONFIG') ?: '';
$ldPreload = getenv('TEST_PHP_APACHE2HANDLER_LD_PRELOAD');
$conf = $root . '/httpd.conf';

file_put_contents($conf, <<<CONF
ServerRoot "$root"
ServerName localhost
Listen 127.0.0.1:$port
PidFile "$root/httpd.pid"
ErrorLog "$root/error.log"
LogLevel warn
LoadModule $moduleName "$module"
$extraConfig
PHPIniDir "$root"

<VirtualHost 127.0.0.1:$port>
    ServerName alpha.local
    DocumentRoot "$alphaRoot"
    <Directory "$alphaRoot">
        Require all granted
        AllowOverride None
    </Directory>
    <FilesMatch "\\.php$">
        SetHandler application/x-httpd-php
    </FilesMatch>
</VirtualHost>

<VirtualHost 127.0.0.1:$port>
    ServerName beta.local
    DocumentRoot "$betaRoot"
    <Directory "$betaRoot">
        Require all granted
        AllowOverride None
    </Directory>
    <FilesMatch "\\.php$">
        SetHandler application/x-httpd-php
    </FilesMatch>
</VirtualHost>
CONF);

try {
    $env = null;
    if ($ldPreload !== false && $ldPreload !== '') {
        /* Sanitizer builds need the runtime preloaded by httpd, but not by the PHP test runner. */
        $env = getenv();
        $env['LD_PRELOAD'] = $ldPreload;
    }

    $process = proc_open(
        [$httpd, '-X', '-f', $conf],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes,
        $root,
        $env
    );
    if (!is_resource($process)) {
        throw new RuntimeException('Unable to start httpd');
    }
    stream_set_blocking($pipes[1], false);
    stream_set_blocking($pipes[2], false);

    user_cache_apache_wait($process, $pipes, $port);

    $checks = [
        ['alpha.local', '/index.php?action=seed', 'alpha.local:alpha.local-value:default'],
        ['alpha.local', '/index.php?action=fetch', 'alpha.local:alpha.local-value:default'],
        ['beta.local', '/index.php?action=fetch', 'beta.local:MISS:default'],
        ['beta.local', '/index.php?action=seed', 'beta.local:beta.local-value:default'],
        ['alpha.local', '/index.php?action=fetch', 'alpha.local:alpha.local-value:default'],
        ['beta.local', '/index.php?action=fetch', 'beta.local:beta.local-value:default'],
    ];

    foreach ($checks as [$host, $path, $expected]) {
        $actual = user_cache_apache_request($port, $host, $path);
        if ($actual !== $expected) {
            throw new RuntimeException("Expected $expected, got $actual");
        }
    }

    echo "Done\n";
} finally {
    if (is_resource($process)) {
        proc_terminate($process);
        proc_close($process);
    }
    user_cache_apache_rm_rf($root);
}

?>
--EXPECT--
Done
