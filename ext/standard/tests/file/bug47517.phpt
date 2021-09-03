--TEST--
Bug #47517 test registry virtualization disabled
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
exec('net session 2>&1', $out, $status);
if (!$status) {
    die('skip test runs under an elevated user account');
}
?>
--FILE--
<?php
/* This has to behave same way on both 64- and 32-bits. */
file_put_contents('C:\Program Files\myfile.txt', 'hello');
?>
--EXPECTF--
Warning: file_put_contents(C:\Program Files\myfile.txt): Failed to open stream: Permission denied in %sbug47517.php on line %d
