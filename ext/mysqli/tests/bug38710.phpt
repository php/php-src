--TEST--
Bug #38710 (data leakage because of nonexisting boundary checking in statements)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "connect.inc";

$db = new mysqli($host, $user, $passwd, "test");
$qry=$db->stmt_init();
$qry->prepare("SELECT REPEAT('a',100000)");
$qry->execute();
$qry->bind_result($text);
$qry->fetch();
var_dump(strlen($text), md5($text));
?>
--EXPECTF--	
int(100000)
string(32) "1af6d6f2f682f76f80e606aeaaee1680"
