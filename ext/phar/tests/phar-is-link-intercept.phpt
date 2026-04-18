--TEST--
phar: is_link() intercept correctly delegates for non-symlink phar entries
--EXTENSIONS--
phar
--SKIPIF--
<?php if (defined('PHP_WINDOWS_VERSION_MAJOR')) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$phar = new Phar($fname);
$phar->addFromString('file.txt', 'hello');
$phar->setStub('<?php
Phar::interceptFileFuncs();
// relative path resolves via phar intercept
var_dump(is_link("file.txt"));       // false: regular entry, not a symlink
var_dump(is_link("nonexistent.txt")); // false: missing entry
// absolute phar:// path bypasses the relative-path intercept, goes to orig_is_link
var_dump(is_link("phar://" . __FILE__ . "/file.txt")); // false: phar entries are never OS symlinks
__HALT_COMPILER(); ?>');
include $fname;
?>
--CLEAN--
<?php @unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECT--
bool(false)
bool(false)
bool(false)
