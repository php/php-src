--TEST--
Zend Opcache: Check zend.portable_build system_id for file cache
--SKIPIF--
<?php
// Ensure the cache directory exists BEFORE OPcache needs it
$cacheDir = __DIR__ . '/portable_build_cache';
if (!is_dir($cacheDir)) {
    @mkdir($cacheDir, 0777, true);
}
// Check if mkdir failed potentially due to permissions
if (!is_dir($cacheDir) || !is_writable($cacheDir)) {
    die('skip Could not create or write to cache directory: ' . $cacheDir);
}
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/portable_build_cache"
--EXTENSIONS--
opcache
--FILE--
<?php
$cache_dir = __DIR__ . '/portable_build_cache';
$test_file = __FILE__;

$ini_settings_portable = [
    'zend_extension'                    => 'opcache',
    'opcache.enable'                    => 1,
    'opcache.enable_cli'                => 1,
    'opcache.jit'                       => 'disable',
    'opcache.file_cache'                => $cache_dir,
    'opcache.file_update_protection'    => 0,
    'zend.portable_build'               => 1,
];

$ini_str_portable = '';
foreach ($ini_settings_portable as $key => $value) {
    $ini_str_portable .= "-d " . escapeshellarg("$key=$value") . " ";
}

$php_executable = getenv('TEST_PHP_EXECUTABLE');
$command_portable = "$php_executable $ini_str_portable -r 'opcache_compile_file(\"$test_file\"); echo opcache_get_status()[\"system_id\"];'";
$portable_system_id = trim(shell_exec($command_portable));

if (substr(PHP_OS, 0, 3) === 'WIN') {
    $pattern = $cache_dir . DIRECTORY_SEPARATOR . $portable_system_id . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . str_replace(':', '', __FILE__) . '.bin';
} else {
    $pattern = $cache_dir . DIRECTORY_SEPARATOR . $portable_system_id . DIRECTORY_SEPARATOR . __FILE__ . '.bin';
}
$cache_files = glob($pattern);
if (count($cache_files) !== 1) {
    die('Failed to find the generated cache file in ' . $cache_dir);
}
$cache_file_path = $cache_files[0];
$cache_file_handle = fopen($cache_file_path, 'rb');
fread($cache_file_handle, 8); // skip the first 8 bytes
$header = fread($cache_file_handle, 32); // Read the first 32 bytes (the system_id)
fclose($cache_file_handle);
$stored_system_id = trim($header);

var_dump($portable_system_id === $stored_system_id);

$default_system_id = opcache_get_status()['system_id'];

var_dump($portable_system_id === $default_system_id);

?>
--CLEAN--
<?php
require __DIR__ . '/cleanup_helper.inc';

$cacheDir = __DIR__ . '/portable_build_cache';

removeDirRecursive($cacheDir);
?>
--EXPECT--
bool(true)
bool(false)
