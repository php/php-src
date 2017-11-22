--TEST--
Bug #62046 	mysqli@mysqlnd can't iterate over stored sets after call to mysqli_stmt_reset()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}
	if (FALSE === ($stmt = $link->prepare('SELECT 42'))) {
		printf("[002] Prepare failed, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	if (FALSE === $stmt->execute()) {
		printf("[003] Execute failed, [%d] %s\n", $stmt->errorno, $stmt->error);	
	}
	if (FALSE === $stmt->store_result()) {
		printf("[004] store_result failed, [%d] %s\n", $stmt->errorno, $stmt->error);	
	}
	$one = NULL;
	if (FALSE === $stmt->bind_result($one)) {
		printf("[005] bind_result failed, [%d] %s\n", $stmt->errorno, $stmt->error);	
	}
	if (FALSE === $stmt->reset()) {
		printf("[006] bind_result failed, [%d] %s\n", $stmt->errorno, $stmt->error);
	}
	while ($stmt->fetch()) {
		var_dump($one);
	}
	$stmt->close();
	$link->close();
	echo "done!";
?>
--EXPECT--
int(42)
done!