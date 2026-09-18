--TEST--
UserCache\Cache: shm size clamp emits a PHP warning
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=0
opcache.file_cache_only=0
opcache.log_verbosity_level=1
user_cache.shm_size=32768M
user_cache.entries_hint=1024
--FILE--
<?php
/* The clamp happens while the INI is parsed and the status reports the
 * clamped size even while the cache is off for the CLI, so nothing here has
 * to back the ~16G mapping. Windows commits the whole mapping up front,
 * which a CI runner cannot satisfy. */
$configured = UserCache\Cache::getStatus()->getConfiguredMemory();
var_dump($configured > 16383 * 1024 ** 2 && $configured < 16384 * 1024 ** 2);
?>
--EXPECTF--
Warning: user_cache.shm_size is limited to slightly under 16384M; clamping in Unknown on line 0
bool(true)
