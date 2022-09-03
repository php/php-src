--TEST--
File cache error 001
--EXTENSIONS--
opcache
posix
pcntl
--INI--
opcache.enable_cli=1
opcache.file_cache={TMP}
opcache.log_verbosity_level=2
--SKIPIF--
<?php
if (!posix_setrlimit(POSIX_RLIMIT_FSIZE, 1, 1)) die('skip Test requires setrlimit(RLIMIT_FSIZE) to work');
if (ini_parse_quantity(ini_get('opcache.jit_buffer_size')) !== 0) die('skip File cache is disabled when JIT is on');
?>
--FILE--
<?php

// Create new file to ensure that it's not cached accross test runs
$file = tempnam(sys_get_temp_dir(), 'file_cache_error');
register_shutdown_function(function () use ($file) {
    unlink($file);
});
file_put_contents($file, '<?php echo "OK";');
touch($file, time() - 3600);

// Some systems will raise SIGXFSZ when RLIMIT_FSIZE is exceeded
if (defined('SIGXFSZ')) {
    pcntl_signal(SIGXFSZ, SIG_IGN);
}

// Should cause writing to cache file to fail
var_dump(posix_setrlimit(POSIX_RLIMIT_FSIZE, 1, 1));

// Will attempt to write to cache file, and fail
require $file;
?>
--EXPECTF--
bool(true)
%sWarning opcache cannot write to file %s: %s

OK
