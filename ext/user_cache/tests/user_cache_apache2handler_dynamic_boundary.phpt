--TEST--
UserCache\Cache: apache2handler isolates automatically mapped mass virtual hosts
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

$builtIn = shell_exec(escapeshellarg($httpd) . ' -l 2>/dev/null') ?: '';
if (str_contains($builtIn, 'mod_vhost_alias.c')) return;

$candidates = [
    getenv('TEST_PHP_APACHE2HANDLER_VHOST_ALIAS_MODULE') ?: null,
    dirname(dirname($httpd)) . '/modules/mod_vhost_alias.so',
    '/usr/libexec/apache2/mod_vhost_alias.so',
    '/usr/lib/apache2/modules/mod_vhost_alias.so',
    '/usr/lib64/httpd/modules/mod_vhost_alias.so',
];
foreach ($candidates as $candidate) {
    if ($candidate !== null && is_file($candidate)) return;
}
die('skip mod_vhost_alias is not built in and its module path is unavailable');
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
        if ($entry !== '.' && $entry !== '..') {
            user_cache_apache_rm_rf($path . DIRECTORY_SEPARATOR . $entry);
        }
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

$root = sys_get_temp_dir() . '/php-user-cache-apache-dynamic-' . getmypid();
$sites = $root . '/sites';
$process = null;
$pipes = [];

user_cache_apache_rm_rf($root);
foreach (['alpha.local', 'beta.local'] as $host) {
    mkdir($sites . '/' . $host, 0777, true);
    file_put_contents($sites . '/' . $host . '/index.php', <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('default');
$host = $_SERVER['HTTP_HOST'] ?? 'unknown';
$action = $_GET['action'] ?? 'fetch';
if ($action === 'clear') {
    $cache->clear();
} elseif ($action === 'seed') {
    $cache->store('dynamic-boundary-key', $host . '-value');
}
echo $host, ':', $cache->fetch('dynamic-boundary-key', 'MISS'), ':', UserCache\Cache::getStatus()->getAvailability()->name, "\n";
PHP);
}
file_put_contents($root . '/php.ini', implode("\n", [
    'user_cache.enable=1',
    'user_cache.shm_size=1M',
    'opcache.file_update_protection=0',
]));

$httpd = getenv('TEST_PHP_APACHE2HANDLER_HTTPD');
$module = getenv('TEST_PHP_APACHE2HANDLER_MODULE');
$moduleName = getenv('TEST_PHP_APACHE2HANDLER_MODULE_NAME') ?: 'php_module';
$extraConfig = getenv('TEST_PHP_APACHE2HANDLER_EXTRA_CONFIG') ?: '';
$ldPreload = getenv('TEST_PHP_APACHE2HANDLER_LD_PRELOAD');
$builtIn = shell_exec(escapeshellarg($httpd) . ' -l 2>/dev/null') ?: '';
$vhostAliasLoad = '';
if (!str_contains($builtIn, 'mod_vhost_alias.c')) {
    $candidates = [
        getenv('TEST_PHP_APACHE2HANDLER_VHOST_ALIAS_MODULE') ?: null,
        dirname(dirname($httpd)) . '/modules/mod_vhost_alias.so',
        '/usr/libexec/apache2/mod_vhost_alias.so',
        '/usr/lib/apache2/modules/mod_vhost_alias.so',
        '/usr/lib64/httpd/modules/mod_vhost_alias.so',
    ];
    foreach ($candidates as $candidate) {
        if ($candidate !== null && is_file($candidate)) {
            $vhostAliasLoad = 'LoadModule vhost_alias_module "' . $candidate . '"';
            break;
        }
    }
}

$port = user_cache_apache_free_port();
$conf = $root . '/httpd.conf';
file_put_contents($conf, <<<CONF
ServerRoot "$root"
ServerName localhost
Listen 127.0.0.1:$port
PidFile "$root/httpd.pid"
ErrorLog "$root/error.log"
LogLevel warn
$vhostAliasLoad
LoadModule $moduleName "$module"
$extraConfig
PHPIniDir "$root"
UseCanonicalName Off
VirtualDocumentRoot "$sites/%0"

<Directory "$sites">
    Require all granted
    AllowOverride None
</Directory>
<FilesMatch "\\.php$">
    SetHandler application/x-httpd-php
</FilesMatch>
CONF);

try {
    $env = null;
    if ($ldPreload !== false && $ldPreload !== '') {
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

    for ($i = 0; $i < 50; $i++) {
        try {
            user_cache_apache_request($port, 'alpha.local', '/index.php?action=fetch');
            break;
        } catch (Throwable) {
            usleep(100000);
        }
    }

    user_cache_apache_request($port, 'alpha.local', '/index.php?action=clear');
    user_cache_apache_request($port, 'beta.local', '/index.php?action=clear');

    $checks = [
        ['alpha.local', '/index.php?action=seed', 'alpha.local:alpha.local-value:Available'],
        ['beta.local', '/index.php?action=fetch', 'beta.local:MISS:Available'],
        ['beta.local', '/index.php?action=seed', 'beta.local:beta.local-value:Available'],
        ['alpha.local', '/index.php?action=fetch', 'alpha.local:alpha.local-value:Available'],
        ['beta.local', '/index.php?action=fetch', 'beta.local:beta.local-value:Available'],
    ];
    foreach ($checks as [$host, $path, $expected]) {
        $actual = user_cache_apache_request($port, $host, $path);
        if ($actual !== $expected) {
            throw new RuntimeException("Expected $expected, got $actual\n" . stream_get_contents($pipes[2]));
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
