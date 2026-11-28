--TEST--
Linux ZTS chdir changes the native working directory
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux' || !PHP_ZTS || !function_exists('pcntl_unshare')) {
    die('skip requires Linux ZTS with unshare');
}
if (!@pcntl_unshare(0x200 /* CLONE_FS */) || !is_link('/proc/thread-self/cwd')) {
    die('skip requires per-thread filesystem state and procfs');
}
?>
--FILE--
<?php
chdir(__DIR__ . '/../file/gh13204[brackets]');
var_dump(getcwd() === readlink('/proc/thread-self/cwd'));
var_dump(file_get_contents('empty.txt'));
$files = new GlobIterator('*.txt');
var_dump($files->current()->getFilename());
?>
--EXPECT--
bool(true)
string(0) ""
string(9) "empty.txt"
