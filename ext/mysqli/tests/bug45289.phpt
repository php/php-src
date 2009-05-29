--TEST--
Bug #45289 (Bogus store_result on PS)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include("connect.inc");
	require('table.inc');

	$id = 1;
	if (!($stmt = $link->prepare('SELECT id, label FROM test WHERE id=? LIMIT 1')))
		printf("[001] [%d] %s\n", $link->errno, $link->error);

	if (!$stmt->bind_param('i', $id) || !$stmt->execute())
		printf("[002] [%d] %s\n", $stmt->errno, $stmt->error);

	if ($res = $link->store_result()) {
		printf("[003] Can store result!\n");
	} else {
		printf("[003] [%d] %s\n", $link->errno, $link->error);
	}

	var_dump($res->fetch_assoc());
?>
--EXPECTF--
[003] [0%s

Fatal error: Call to a member function fetch_assoc() on a non-object in %s on line %d
