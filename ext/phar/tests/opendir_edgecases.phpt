--TEST--
Phar: test edge cases of opendir() function interception
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.readonly=0
--FILE--
<?php

Phar::interceptFileFuncs();

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

opendir(array());

mkdir(dirname(__FILE__) . '/poo');
chdir(dirname(__FILE__));

$a = opendir('poo');

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
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php rmdir(dirname(__FILE__) . '/poo');
--EXPECTF--
Warning: opendir() expects parameter 1 to be %string, array given in %sopendir_edgecases.php on line %d
.
..
foo

Warning: opendir(phar://%sopendir_edgecases.phar.php/oops): failed to open dir: %s in phar://%sopendir_edgecases.phar.php/foo on line %d
===DONE===