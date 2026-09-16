--TEST--
Tailcall VM is selected when compiled with Clang >= 19 on Windows x64
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
if (php_uname('m') !== 'AMD64') die('skip x64 only');

ob_start();
phpinfo(INFO_GENERAL);
$info = ob_get_clean();

if (!preg_match('/Compiler => clang version (\d+)/', $info, $m)) {
    die('skip not compiled with clang');
}

if ((int)$m[1] < 19) {
    die('skip requires clang >= 19');
}
?>
--FILE--
<?php
var_dump(ZEND_VM_KIND);
?>
--EXPECT--
string(21) "ZEND_VM_KIND_TAILCALL"
