--TEST--
UserCache\Cache: FastCGI boundary partitions follow the normalized DOCUMENT_ROOT directory, ignore SERVER_NAME and HTTP_HOST and stop at the partition limit
--CONFLICTS--
all
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip FastCGI boundary server test is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
require_once __DIR__ . '/user_cache_fcgi_tester.inc';
if (FcgiTester::binary() === null) die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

$server = new FcgiTester('boundary');

try {
    /* Partition key is the DOCUMENT_ROOT directory after normalizing its path; symbolic links are not resolved */
    $script = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('boundary');
$key = 'cgi-boundary-key';
$complexKey = 'cgi-boundary-complex-key';
$action = $_GET['action'] ?? 'fetch';
$host = $_SERVER['SERVER_NAME'] ?? 'unknown';

if ($action === 'clear') {
    $cache->clear();
} elseif ($action === 'seed') {
    $cache->store($key, $host . '-value');
    $cache->store($complexKey, ['host' => $host, 'nested' => ['value' => 42]]);
}

$status = UserCache\Cache::getStatus();
$complex = $cache->fetch($complexKey, ['host' => 'MISS', 'nested' => ['value' => 'MISS']]);
echo $host, ':', $cache->fetch($key, 'MISS'), ':', $complex['host'], ':', $complex['nested']['value'], ':', ($status->getAvailability() === UserCache\CacheAvailability::Available ? 'available' : $status->getAvailability()->name), "\n";
PHP;

    $alphaRoot = $server->docRoot('alpha', $script);
    $betaRoot = $server->docRoot('beta', $script);
    $alphaScript = $alphaRoot . '/index.php';
    $betaScript = $betaRoot . '/index.php';

    $server->start([
        'user_cache.enable=1',
        'user_cache.shm_size=8M',
        'opcache.file_update_protection=0',
    ], $alphaScript, $alphaRoot, 'alpha.local', 'action=fetch');

    $server->request($alphaScript, $alphaRoot, 'alpha.local', 'action=clear');
    $server->request($betaScript, $betaRoot, 'beta.local', 'action=clear');

    $checks = [
        [$alphaScript, $alphaRoot, 'alpha.local', 'action=seed', 'alpha.local:alpha.local-value:alpha.local:42:available'],
        [$alphaScript, $alphaRoot, 'alpha.local', 'action=fetch', 'alpha.local:alpha.local-value:alpha.local:42:available'],
        [$alphaScript, $alphaRoot, 'beta.local', 'action=fetch', 'beta.local:alpha.local-value:alpha.local:42:available'],
        [$alphaScript, $alphaRoot . '/', 'gamma.local', 'action=fetch', 'gamma.local:alpha.local-value:alpha.local:42:available'],
        [$alphaScript, $betaRoot . '/../alpha', 'alpha.local', 'action=fetch', 'alpha.local:alpha.local-value:alpha.local:42:available'],
        [$betaScript, $betaRoot, 'beta.local', 'action=fetch', 'beta.local:MISS:MISS:MISS:available'],
        [$betaScript, $betaRoot, 'beta.local', 'action=seed', 'beta.local:beta.local-value:beta.local:42:available'],
        [$alphaScript, $alphaRoot, 'alpha.local', 'action=fetch', 'alpha.local:alpha.local-value:alpha.local:42:available'],
        [$betaScript, $betaRoot, 'beta.local', 'action=fetch', 'beta.local:beta.local-value:beta.local:42:available'],
        [$alphaScript, $server->path('missing'), 'alpha.local', 'action=fetch', 'alpha.local:MISS:MISS:MISS:UnavailableByCgiFastCgiBoundary'],
        [$alphaScript, 'alpha', 'alpha.local', 'action=fetch', 'alpha.local:MISS:MISS:MISS:UnavailableByCgiFastCgiBoundary'],
        [$alphaScript, $alphaScript, 'alpha.local', 'action=fetch', 'alpha.local:MISS:MISS:MISS:UnavailableByCgiFastCgiBoundary'],
    ];

    foreach ($checks as [$file, $docRoot, $serverName, $query, $expected]) {
        $server->expect($expected, $file, $docRoot, $serverName, $query);
    }

    /* A deployment symlink keeps its partition when it is switched to another release */
    $current = $server->path('current');
    symlink($alphaRoot, $current);
    $server->expect('current.local:MISS:MISS:MISS:available', $alphaScript, $current, 'current.local', 'action=fetch');
    $server->expect('current.local:current.local-value:current.local:42:available', $alphaScript, $current, 'current.local', 'action=seed');
    unlink($current);
    symlink($betaRoot, $current);
    $server->expect('current.local:current.local-value:current.local:42:available', $betaScript, $current, 'current.local', 'action=fetch');
    unlink($current);

    /* Neither HTTP_HOST nor SERVER_NAME selects the partition */
    $siteRoot = $server->docRoot('site', <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('http-host');
$action = $_GET['action'] ?? 'fetch';
if ($action === 'clear') {
    $cache->clear();
} elseif ($action === 'seed') {
    $cache->store('key', 'seeded-value');
}
$availability = UserCache\Cache::getStatus()->getAvailability();
echo $cache->fetch('key', 'MISS'), ':', ($availability === UserCache\CacheAvailability::Available ? 'available' : $availability->name), "\n";
PHP);
    $siteScript = $siteRoot . '/index.php';

    $checks = [
        ['vhost.local', 'a000001.example', 'action=clear', 'MISS:available'],
        ['vhost.local', 'a000001.example', 'action=seed',  'seeded-value:available'],
        ['vhost.local', 'a000002.example', 'action=fetch', 'seeded-value:available'],
        ['vhost.local', 'a000003.example', 'action=fetch', 'seeded-value:available'],
        ['other.local', 'a000001.example', 'action=fetch', 'seeded-value:available'],
        ['vhost.local', 'a999999.example', 'action=fetch', 'seeded-value:available'],
    ];

    foreach ($checks as [$serverName, $httpHost, $query, $expected]) {
        $server->expect($expected, $siteScript, $siteRoot, $serverName, $query, $httpHost);
    }
} finally {
    $server->cleanup();
}

/* The 33rd distinct boundary is refused and logged exactly once */
$limit = new FcgiTester('boundary-limit');

try {
    $source = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('boundary-limit');
$action = $_GET['action'] ?? 'fetch';
$host = $_SERVER['SERVER_NAME'] ?? 'unknown';
if ($action === 'seed') {
    $cache->store('key', $host . '-value');
}
$availability = UserCache\Cache::getStatus()->getAvailability();
echo $host, ':', $cache->fetch('key', 'MISS'), ':', $availability->name, "\n";
PHP;
    $docRoots = [];
    for ($i = 1; $i <= 34; $i++) {
        $docRoots[$i] = $limit->docRoot(sprintf('site%02d', $i), $source);
    }
    $script = $docRoots[1] . '/index.php';
    $log = $limit->path('error.log');

    $limit->start([
        'user_cache.enable=1',
        'user_cache.shm_size=1M',
        'opcache.file_update_protection=0',
        'display_errors=0',
        'log_errors=1',
        'error_log=' . $log,
    ], $script, $docRoots[1], 'host01.local', 'action=fetch');

    $limit->expect('host01.local:host01.local-value:Available', $script, $docRoots[1], 'host01.local', 'action=seed');

    for ($i = 2; $i <= 32; $i++) {
        $host = sprintf('host%02d.local', $i);
        $limit->expect($host . ':MISS:Available', $script, $docRoots[$i], $host, 'action=fetch');
    }

    foreach ([33, 34] as $i) {
        $host = sprintf('host%02d.local', $i);
        $limit->expect($host . ':MISS:UnavailableByCgiFastCgiBoundary', $script, $docRoots[$i], $host, 'action=seed');
    }

    $limit->expect('host01.local:host01.local-value:Available', $script, $docRoots[1], 'host01.local', 'action=fetch');

    $contents = is_file($log) ? file_get_contents($log) : '';
    $needle = 'UserCache: boundary partition limit (32) reached; creation of new partitions has been disabled';
    if (substr_count($contents, $needle) !== 1) {
        throw new RuntimeException("Expected one boundary-limit error-log entry, got:\n" . $contents);
    }

    echo "Done\n";
} finally {
    $limit->cleanup();
}

?>
--EXPECT--
Done
