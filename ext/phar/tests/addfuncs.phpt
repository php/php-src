--TEST--
Phar: addFile/addFromString
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$phar = new Phar($fname);
$phar->addFromString('a', 'hi');
echo file_get_contents($pname . '/a') . "\n";
$phar->addFile($pname . '/a', 'b');
echo file_get_contents($pname . '/b') . "\n";
try {
$phar->addFile($pname . '/a');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
$phar->addFile($pname . '/a', 'a');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
$phar->addFile(__DIR__ . '/does/not/exist');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
$phar->addFile($pname . '/a', '.phar/stub.php');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
$phar->addFromString('.phar/stub.php', 'hi');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
hi
hi
BadMethodCallException: Entry phar://%saddfuncs.phar.php/a does not exist and cannot be created: phar error: invalid path "phar://%saddfuncs.phar.php/a" contains double slash
BadMethodCallException: Entry a does not exist and cannot be created: phar error: file "a" in phar "%saddfuncs.phar.php" cannot be opened for writing, readable file pointers are open
RuntimeException: phar error: unable to open file "%s/does/not/exist" to add to phar archive
BadMethodCallException: Cannot create any files in magic ".phar" directory
BadMethodCallException: Cannot create any files in magic ".phar" directory
