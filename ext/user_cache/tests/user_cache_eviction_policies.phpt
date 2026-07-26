--TEST--
UserCache\Cache: user_cache.eviction_policy selects lru, clear or none behavior
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=4M
--FILE--
<?php
$php = escapeshellarg(getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY);
$args = '-n -d user_cache.enable=1 -d user_cache.enable_cli=1 -d user_cache.shm_size=4M';
$code = <<<'CODE'
$cache = UserCache\Cache::getPool('p');
$blob = str_repeat('x', 8192);
$ok = 0;
for ($i = 0; $i < 600; $i++) {
    if ($cache->store('k' . $i, $blob . $i)) {
        $ok++;
    }
}
$status = UserCache\Cache::getStatus();
printf(
    "stored=%d full=%d evicted=%d wiped=%d failed=%d\n",
    $ok,
    (int) ($status->getEntryCount() > 400),
    (int) ($status->getEvictionCount() > 0),
    (int) ($status->getExpungeCount() > 0),
    (int) ($status->getStoreFailureCount() > 0)
);
CODE;

foreach (['lru', 'clear', 'none'] as $policy) {
    echo $policy, ': ', shell_exec(
        "$php $args -d user_cache.eviction_policy=$policy -r " . escapeshellarg($code)
    );
}

/* Invalid values are rejected at INI time and fall back to the lru default. */
echo shell_exec("$php $args -d user_cache.eviction_policy=bogus -r " . escapeshellarg('echo "rejected\n";') . " 2>&1");
?>
--EXPECTF--
lru: stored=600 full=1 evicted=1 wiped=0 failed=0
clear: stored=600 full=0 evicted=0 wiped=1 failed=0
none: stored=%d full=1 evicted=0 wiped=0 failed=1
%Auser_cache.eviction_policy must be one of "lru", "clear" or "none"%Arejected
