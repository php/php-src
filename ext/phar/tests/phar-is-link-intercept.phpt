--TEST--
phar: is_link() intercept correctly delegates for non-symlink phar entries
--EXTENSIONS--
phar
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
echo "regular entry (not a symlink): ";
var_dump(is_link("file.txt"));
echo "missing entry: ";
var_dump(is_link("nonexistent.txt"));
echo "absolute phar:// path (bypasses intercept): ";
var_dump(is_link("phar://" . __FILE__ . "/file.txt"));
__HALT_COMPILER(); ?>');
include $fname;
?>
--CLEAN--
<?php @unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECT--
regular entry (not a symlink): bool(false)
missing entry: bool(false)
absolute phar:// path (bypasses intercept): bool(false)
