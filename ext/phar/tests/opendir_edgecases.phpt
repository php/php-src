--TEST--
Phar: test edge cases of opendir() function interception
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.readonly=0
--FILE--
<?php

Phar::interceptFileFuncs();

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

try {
    opendir(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

mkdir(__DIR__ . '/opendir_edgecases');
chdir(__DIR__);

$a = opendir('opendir_edgecases');

$arr = array();
while (false !== ($b = readdir($a))) {
    $arr[] = $b;
}
sort($arr);
foreach ($arr as $b) {
    echo "$b\n";
}

closedir($a);

file_put_contents($pname . '/foo', '<?php
$context = stream_context_create();
$a = opendir(".", $context);
$res = array();
while (false !== ($b = readdir($a))) {
$res[] = $b;
}
sort($res);
foreach ($res as $b) {
echo "$b\n";
}
opendir("oops");
?>');

include $pname . '/foo';

?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php rmdir(__DIR__ . '/opendir_edgecases');
--EXPECTF--
opendir(): Argument #1 ($directory) must be of type string, array given
.
..
foo

Warning: opendir(phar://%sopendir_edgecases.phar.php/oops): Failed to open directory: %s in phar://%sopendir_edgecases.phar.php/foo on line %d
