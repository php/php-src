--TEST--
UserCache\Cache: user_cache.entries_hint sizes the entry table
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
user_cache.entries_hint=1000
--FILE--
<?php
/* capacity = next_prime(ceil(hint / 0.75)) = next_prime(1334) */
var_dump(UserCache\Cache::getStatus()->getEntryCapacity());

/* Auto sizing (hint 0): one expected entry per 2KB of segment. */
$php = escapeshellarg(getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY);
$args = '-n -d user_cache.enable=1 -d user_cache.enable_cli=1 -d user_cache.shm_size=16M';
$code = 'echo UserCache\\Cache::getStatus()->getEntryCapacity();';
echo shell_exec("$php $args -d user_cache.entries_hint=0 -r " . escapeshellarg($code)), "\n";

/* A negative hint is rejected at INI time and falls back to auto. */
echo shell_exec("$php $args -d user_cache.entries_hint=-1 -r " . escapeshellarg($code) . " 2>&1"), "\n";

/* A hint the segment cannot index is clamped with a warning. */
echo shell_exec("$php $args -d user_cache.entries_hint=16777213 -r " . escapeshellarg($code) . " 2>&1"), "\n";
?>
--EXPECTF--
int(1361)
10937
%Auser_cache.entries_hint must be greater than or equal to 0, -1 given%A10937
%Auser_cache.entries_hint (16777213) exceeds what user_cache.shm_size can index; clamping capacity to %d%A
