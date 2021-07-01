--TEST--
Phar: PharFileInfo::chmod extra code coverage
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);

$phar['a/b'] = 'hi there';

$b = $phar['a/b'];
try {
$phar['a']->chmod(066);
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
lstat($pname . '/a/b'); // sets BG(CurrentLStatFile)
$b->chmod(0666);
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECT--
Phar entry "a" is a temporary directory (not an actual entry in the archive), cannot chmod
