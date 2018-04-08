--TEST--
Bug #38579 (include_once() may include the same file twice)
--SKIPIF--
<?php
if(PHP_OS_FAMILY !== "WIN") {
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
