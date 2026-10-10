--TEST--
UserCache\Cache: apache2handler creates each virtual host's partition with the php_admin_value / php_admin_flag settings of that virtual host
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
            user_cache_apache_request($port, 'alpha.local', '/index.php');
            return;
        } catch (Throwable) {
            usleep(100000);
        }
    }

    throw new RuntimeException(stream_get_contents($pipes[2]));
}

$root = sys_get_temp_dir() . '/php-user-cache-apache-vhost-ini-' . getmypid();
$process = null;
$pipes = [];

user_cache_apache_rm_rf($root);

$script = <<<'PHP'
<?php
$status = UserCache\Cache::getStatus();
echo implode(':', [
    $_SERVER['SERVER_NAME'],
    $status->getAvailability()->name,
    $status->getConfiguredMemory() >> 20,
    $status->getSharedMemorySize() >> 20,
    $status->getEntryCapacity(),
]), "\n";
PHP;

$vhosts = [
    'alpha' => "php_admin_value user_cache.shm_size 4M\n    php_admin_value user_cache.entries_hint 127",
    'beta' => '',
    'gamma' => 'php_value user_cache.shm_size 2M',
    'delta' => 'php_admin_flag user_cache.enable off',
];
$vhostConfig = '';
foreach ($vhosts as $name => $settings) {
    mkdir("$root/$name", 0777, true);
    file_put_contents("$root/$name/index.php", $script);
    $vhostConfig .= <<<CONF

<VirtualHost 127.0.0.1:{{PORT}}>
    ServerName $name.local
    DocumentRoot "$root/$name"
    $settings
    <Directory "$root/$name">
        Require all granted
        AllowOverride None
    </Directory>
    <FilesMatch "\\.php$">
        SetHandler application/x-httpd-php
    </FilesMatch>
</VirtualHost>
CONF;
}

file_put_contents($root . '/php.ini', implode("\n", [
    'user_cache.enable=1',
    'user_cache.shm_size=8M',
    'user_cache.entries_hint=99999999',
    'log_errors=1',
    'display_errors=0',
    'error_log=' . $root . '/php.log',
]));

$port = user_cache_apache_free_port();
$httpd = getenv('TEST_PHP_APACHE2HANDLER_HTTPD');
$module = getenv('TEST_PHP_APACHE2HANDLER_MODULE');
$moduleName = getenv('TEST_PHP_APACHE2HANDLER_MODULE_NAME') ?: 'php_module';
$extraConfig = getenv('TEST_PHP_APACHE2HANDLER_EXTRA_CONFIG') ?: '';
$ldPreload = getenv('TEST_PHP_APACHE2HANDLER_LD_PRELOAD');
$conf = $root . '/httpd.conf';
$vhostConfig = str_replace('{{PORT}}', (string) $port, $vhostConfig);

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
$vhostConfig
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

    foreach (array_keys($vhosts) as $name) {
        echo user_cache_apache_request($port, "$name.local", '/index.php'), "\n";
    }

    echo "Warnings after several requests:\n";
    for ($i = 0; $i < 3; $i++) {
        user_cache_apache_request($port, 'alpha.local', '/index.php');
    }
    $masterClamp = 'user_cache.entries_hint is limited to 16777213; clamping';
    $capacityClamp = 'user_cache.entries_hint (16777213) exceeds what user_cache.shm_size can index';
    $errorLog = (string) file_get_contents("$root/php.log");
    $httpdLog = (string) file_get_contents("$root/error.log");
    echo 'error_log clamp warnings: ', substr_count($errorLog, $masterClamp), ', capacity clamp warnings: ', substr_count($errorLog, $capacityClamp), "\n";
    echo 'httpd error log capacity clamp warnings: ', substr_count($httpdLog, $capacityClamp), "\n";
} finally {
    if (is_resource($process)) {
        proc_terminate($process);
        proc_close($process);
    }
    user_cache_apache_rm_rf($root);
}

?>
--EXPECTF--
alpha.local:Available:4:4:173
beta.local:Available:8:8:%d
gamma.local:Available:8:8:%d
delta.local:DisabledByIni:8:0:0
Warnings after several requests:
error_log clamp warnings: 1, capacity clamp warnings: 0
httpd error log capacity clamp warnings: 1
