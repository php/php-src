--TEST--
Bug #38710 (data leakage because of nonexisting boundary checking in statements)
--SKIPIF--
<?php 
require_once('skipif.inc'); 
?>
--FILE--
<?php
include "connect.inc";

$db = new mysqli($host, $user, $passwd, "test");
$qry=$db->stmt_init();
$qry->prepare("SELECT REPEAT('a',100000)");
$qry->execute();
$qry->bind_result($text);
$qry->fetch();
if ($text !== str_repeat('a', mysqli_get_server_version($db) > 50110? 100000:(mysqli_get_server_version($db)>=50000? 8193:8191))) {
	var_dump(strlen($text));
}
echo "Done";
?>
--EXPECTF--	
Done
