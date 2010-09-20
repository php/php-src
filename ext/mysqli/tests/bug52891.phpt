--TEST--
Bug #52891 (Wrong data inserted with mysqli/mysqlnd when using bind_param,value > LONG_MAX)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");
	$link = mysqli_init();
	if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
		printf("[003] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}
	var_dump($link->query("DROP TABLE IF EXISTS tuint"));
	var_dump($link->query("DROP TABLE IF EXISTS tsint"));
	var_dump($link->query("CREATE TABLE tuint(a bigint unsigned) ENGINE=".$engine));
	var_dump($link->query("CREATE TABLE tsint(a bigint) ENGINE=".$engine));
	
	$stmt = $link->prepare("INSERT INTO tuint VALUES(?)");
	$stmt2 = $link->prepare("INSERT INTO tsint VALUES(?)");
	$param = 42;
	$stmt->bind_param("i", $param);
	$stmt2->bind_param("i", $param);

	/* first insert normal value to force initial send of types */
	var_dump($stmt->execute());
	var_dump($stmt2->execute());

	/* now try values that don't fit in long, on 32bit, new types should be sent or 0 will be inserted */
	$param = -4294967297;
	var_dump($stmt2->execute());

	/* again normal value */
	$param = 43;
	var_dump($stmt->execute());
	var_dump($stmt2->execute());

	/* again conversion */
	$param = -4294967295;
	var_dump($stmt2->execute());

	$param = 4294967295;
	var_dump($stmt->execute());
	var_dump($stmt2->execute());

	$param = 4294967297;
	var_dump($stmt->execute());
	var_dump($stmt2->execute());

	$result = $link->query("SELECT * FROM tsint ORDER BY a");
	$result2 = $link->query("SELECT * FROM tuint ORDER BY a");

	var_dump($link->query("DROP TABLE tsint"));
	var_dump($link->query("DROP TABLE tuint"));

	echo "tsint:\n";
	while ($row = $result->fetch_assoc()) {
		var_dump($row);
	}
	echo "tuint:\n";
	while ($row = $result2->fetch_assoc()) {
		var_dump($row);
	}
	
	echo "done";
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
tsint:
array(1) {
  ["a"]=>
  string(11) "-4294967297"
}
array(1) {
  ["a"]=>
  string(11) "-4294967295"
}
array(1) {
  ["a"]=>
  string(2) "42"
}
array(1) {
  ["a"]=>
  string(2) "43"
}
array(1) {
  ["a"]=>
  string(10) "4294967295"
}
array(1) {
  ["a"]=>
  string(10) "4294967297"
}
tuint:
array(1) {
  ["a"]=>
  string(2) "42"
}
array(1) {
  ["a"]=>
  string(2) "43"
}
array(1) {
  ["a"]=>
  string(10) "4294967295"
}
array(1) {
  ["a"]=>
  string(10) "4294967297"
}
done
