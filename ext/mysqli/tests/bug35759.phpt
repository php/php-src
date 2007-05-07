--TEST--
Bug #35759 (mysqli_stmt_bind_result() makes huge allocation when column empty)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$sql=<<<EOSQL
CREATE TABLE blobby (
  a1 MEDIUMBLOB NOT NULL,


EOSQL;
	include "connect.inc";
	$col_num= 1000;

	$mysql = new mysqli($host, $user, $passwd, "test");
	$mysql->query("DROP TABLE IF EXISTS blobby");
	$create = "CREATE TABLE blobby (a0 MEDIUMBLOB NOT NULL DEFAULT ''";
	$i= 0;
	while (++$i < $col_num) {
		$create .= ", a$i MEDIUMBLOB NOT NULL DEFAULT ''";
	}
        $create .= ")";	
          
        $mysql->query($create);
	$mysql->query("INSERT INTO blobby (a0) VALUES ('')");
	
	$stmt = $mysql->prepare("SELECT * FROM blobby");
	$stmt->execute();
	$stmt->store_result();
	$params= array_pad(array(), $col_num, "");
	call_user_func_array(array($stmt, "bind_result"), $params);
	$stmt->fetch();
	
	$stmt->close();

	$mysql->query("DROP TABLE blobby");

	$mysql->close();
	echo "OK\n";
?>
--EXPECT--
OK
