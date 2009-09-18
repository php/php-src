--TEST--
Bug #48909 (Segmentation fault in mysqli_stmt_execute)
--SKIPIF--
<?php 
require_once('skipif.inc'); 
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	include "table.inc";
	
	$stmt = $link->prepare("insert into test values (?,?)");
	var_dump($stmt->bind_param("bb",$bvar,$bvar));
	var_dump($stmt->execute());
	$stmt->close();
	$link->close();
	echo "done";	
?>
--EXPECTF--
bool(true)
bool(true)
done
