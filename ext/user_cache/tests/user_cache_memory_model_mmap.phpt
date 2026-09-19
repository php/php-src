--TEST--
UserCache\Cache: end-to-end on the explicit "mmap" memory model
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
user_cache.preferred_memory_model=mmap
--SKIPIF--
<?php
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$probe = 'UserCache\Cache::getPool("probe")->store("k", 1);'
    . 'ob_start(); phpinfo(INFO_MODULES);'
    . 'preg_match("/^Active memory model => (.+)$/m", ob_get_clean(), $m);'
    . 'echo trim($m[1] ?? "none");';
$cmd = escapeshellarg($php)
    . ' -n -d user_cache.enable=1 -d user_cache.enable_cli=1 -d user_cache.shm_size=16M'
    . ' -d user_cache.preferred_memory_model=mmap'
    . ' -r ' . escapeshellarg($probe) . ' 2>/dev/null';
$active = trim((string) shell_exec($cmd));
if ($active !== 'mmap') {
    die("skip mmap memory model unavailable (active: $active)");
}
?>
--FILE--
<?php
require __DIR__ . '/memory_model_exercise.inc';
user_cache_memory_model_exercise('mmap');
?>
--EXPECT--
bool(true)
string(9) "Available"
active model: as requested
bool(true)
int(42)
bool(true)
float(1.5)
bool(true)
bool(false)
bool(true)
NULL
bool(true)
string(5) "hello"
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
string(1) "x"
array(1) {
  [0]=>
  int(1)
}
bool(true)
bool(true)
array(1) {
  ["m"]=>
  int(2)
}
bool(false)
bool(true)
int(7)
bool(true)
bool(false)
bool(true)
int(15)
int(14)
bool(true)
array(3) {
  ["m1"]=>
  string(1) "a"
  ["m2"]=>
  string(1) "b"
  ["m3"]=>
  string(4) "dflt"
}
bool(true)
bool(true)
bool(true)
string(8) "computed"
string(8) "computed"
int(1)
child sees parent data
bool(true)
int(114)
string(11) "child-value"
int(114)
bool(true)
bool(false)
