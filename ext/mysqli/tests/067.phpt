--TEST--
function test: nested selects (cursors)
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifconnectfailure.inc');
	require_once("connect.inc");

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		die("skip Cannot connect to check required version");

	/* skip cursor test for versions < 50004 */
	if ((!$IS_MYSQLND && (mysqli_get_client_version() < 50009)) ||
			(mysqli_get_server_version($link) < 50009)) {
			die(sprintf("skip Client library doesn't support cursors (%s/%s)",
					mysqli_get_client_version(), mysqli_get_server_version($link)));
	}
	mysqli_close($link);
?>
--FILE--
<?php
	function open_cursor($mysql, $query) {
		if (!is_object($stmt = $mysql->prepare($query))) {
			printf("[001] Cannot create statement object for '%s', [%d] %s\n",
					$query, $mysql->errno, $mysql->error);
		}

		$stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY);
		return $stmt;
	}

	require_once("connect.inc");
	$mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

	if ((!$IS_MYSQLND && mysqli_get_client_version() < 50009) ||
		(mysqli_get_server_version($mysql) < 50009)) {
		/* we really want to skip it... */
		die(var_dump(63));
	}

	$a = array();

	for ($i=0;$i < 3; $i++) {
		$mysql->query("DROP TABLE IF EXISTS cursor$i");
		$mysql->query("CREATE TABLE cursor$i (a int not null) ENGINE=" . $engine);
		$mysql->query("INSERT INTO cursor$i VALUES (1),(2),(3),(4),(5),(6)");
		$stmt[$i] = open_cursor($mysql, "SELECT a FROM cursor$i");
		$stmt[$i]->execute();
		$stmt[$i]->bind_result($a[$i]);
	}


	$cnt = 0;
	while ($stmt[0]->fetch()) {
		$stmt[1]->fetch();
		$stmt[2]->fetch();
		$cnt += $a[0] + $a[1] + $a[2];
	}

	for ($i=0; $i < 3; $i++) {
		$stmt[$i]->close();
	}

	$mysql->close();
	var_dump($cnt);
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

for ($i =0; $i < 3; $i++) {
	if (!mysqli_query($link, sprintf("DROP TABLE IF EXISTS cursor%d", $i)))
		printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

mysqli_close($link);
?>
--EXPECT--
int(63)