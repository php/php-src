--TEST--
UserCache\Cache: a boundary segment outlives php-cgi restarts, is only reattached by the same build identity and is removed once idle by a process of another build for the same boundary
--CONFLICTS--
all
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
if (PHP_OS_FAMILY === 'Windows') die('skip FastCGI boundary server test is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
require_once __DIR__ . '/user_cache_fcgi_tester.inc';
if (FcgiTester::binary() === null) die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

$server = new FcgiTester('build-identity');

try {
    $root = $server->docRoot('site', <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('build-identity');
if (($_GET['action'] ?? 'fetch') === 'seed') {
    $cache->store('k', 'seeded');
}
echo $cache->fetch('k', 'MISS');
PHP);
    $script = $root . '/index.php';

    $run = function (bool $otherBuild, string $query) use ($server, $script, $root): string {
        putenv('USER_CACHE_DEBUG_SIMULATE_OTHER_BUILD' . ($otherBuild ? '=1' : ''));
        $server->start(['user_cache.enable=1', 'user_cache.shm_size=1M'], $script, $root, 'identity.local', 'action=fetch');
        $out = $server->request($script, $root, 'identity.local', $query);
        $server->stop();

        return $out;
    };

    $locks = fn(): int => count(glob($server->path('.PhpUserCacheBnd.*/*.lock')) ?: []);

    echo $run(false, 'action=seed'), "\n";
    echo $run(false, 'action=fetch'), "\n";
    echo $run(true, 'action=fetch'), ' ', $locks(), "\n";
    echo $run(false, 'action=fetch'), ' ', $locks(), "\n";
} finally {
    putenv('USER_CACHE_DEBUG_SIMULATE_OTHER_BUILD');
    $server->cleanup();
}
?>
--EXPECT--
seeded
seeded
MISS 1
MISS 1
