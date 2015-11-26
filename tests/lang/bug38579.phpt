--TEST--
Bug #38579 (include_once() may include the same file twice)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
?>
--FILE--
<?php
$file = dirname(__FILE__)."/bug38579.inc";
include_once(strtolower($file));
include_once(strtoupper($file));
?>
--EXPECT--
ok
