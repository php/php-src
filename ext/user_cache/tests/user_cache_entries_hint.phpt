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
$out = shell_exec("$php $args -d user_cache.entries_hint=16777213 -r " . escapeshellarg($code) . " 2>&1");
echo $out, "\n";

/* The clamped table plus its per-key lock region (1024 records at this
 * scale) must still leave half the segment for value data. Entry records
 * are 48 bytes and lock records 40 on 64-bit layouts, 44 and 36 on 32-bit;
 * each entry also carries a 4-byte access stamp. */
[$entry_bytes, $lock_bytes] = PHP_INT_SIZE >= 8 ? [48, 40] : [44, 36];
preg_match('/(\d+)\s*$/', $out, $m);
$cap = (int) $m[1];
var_dump($cap >= 100000, $cap * ($entry_bytes + 4) + 1024 * $lock_bytes <= 8 * 1024 * 1024);
?>
--EXPECTF--
int(1361)
10937
%Auser_cache.entries_hint must be greater than or equal to 0, -1 given%A10937
%Auser_cache.entries_hint (16777213) exceeds what user_cache.shm_size can index; clamping capacity to %d%A
bool(true)
bool(true)
