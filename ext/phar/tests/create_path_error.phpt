--TEST--
Phar: create with illegal path
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
<?php if (!extension_loaded("spl")) print "skip SPL not available"; ?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

@unlink($fname);

file_put_contents($pname . '/a.php?', "query");
file_put_contents($pname . '/b.php?bla', "query");

var_dump(file_get_contents($pname . '/a.php'));
var_dump(file_get_contents($pname . '/b.php'));

function error_handler($errno, $errmsg)
{
	echo "Error: $errmsg\n";
}

set_error_handler('error_handler');

$checks = array('/', '.', '../', 'a/..', 'a/', 'b//a.php');
foreach($checks as $check)
{
	file_put_contents($pname . '/' . $check, "error");
}

?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
string(5) "query"
string(5) "query"
Error: file_put_contents(phar://%s//): failed to open stream: phar error: invalid path "/" contains empty directory
Error: file_put_contents(phar://%s/.): failed to open stream: phar error: invalid path "." contains current directory reference
Error: file_put_contents(phar://%s/../): failed to open stream: phar error: invalid path "../" contains empty directory
Error: file_put_contents(phar://%s/a/..): failed to open stream: phar error: invalid path "a/.." contains upper directory reference
Error: file_put_contents(phar://%s/a/): failed to open stream: phar error: invalid path "a/" contains empty directory
Error: file_put_contents(phar://%s/b//a.php): failed to open stream: phar error: invalid path "b//a.php" contains double slash
===DONE===
