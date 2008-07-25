--TEST--
Bug #45019 (Segmentation fault with SELECT ? and UNION)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");
	require_once("table.inc");

	// Regular (non-prepared) queries
	print "Using CAST('somestring' AS CHAR)...\n";
	if (!($res = $link->query("SELECT CAST('one' AS CHAR) AS column1 UNION SELECT CAST('three' AS CHAR) UNION SELECT CAST('two' AS CHAR)")))
		printf("[001] [%d] %s\n", $link->errno, $link->error);

	$data = array();
	while ($row = $res->fetch_assoc()) {
		$data[] = $row['column1'];
		var_dump($row['column1']);
	}
	$res->free();

	// Prepared Statements
	if (!($stmt = $link->prepare("SELECT CAST('one' AS CHAR) AS column1 UNION SELECT CAST('three' AS CHAR) UNION SELECT CAST('two' AS CHAR)")))
		printf("[002] [%d] %s\n", $link->errno, $link->error);

	$column1 = null;
	if (!$stmt->bind_result($column1) || !$stmt->execute())
		printf("[003] [%d] %s\n", $stmt->errno, $stmt->error);

	$index = 0;
	while ($stmt->fetch()) {
		if ($data[$index] != $column1) {
			printf("[004] Row %d, expecting %s/%s got %s/%s\n",
				$index + 1, gettype($data[$index]), $data[$index], gettype($column1), $column1);
		}
		$index++;
	}
	$stmt->close();

	// Regular (non-prepared) queries
	print "Mixing CAST('somestring'AS CHAR), integer and CAST(integer AS CHAR)...\n";
	if (!($res = $link->query("SELECT 1 AS column1 UNION SELECT CAST('three' AS CHAR) UNION SELECT CAST(2 AS CHAR)")))
		printf("[005] [%d] %s\n", $link->errno, $link->error);

	$data = array();
	while ($row = $res->fetch_assoc()) {
		$data[] = $row['column1'];
	}
	$res->free();

	// Prepared Statements
	if (!($stmt = $link->prepare("SELECT 1 AS column1 UNION SELECT CAST('three' AS CHAR) UNION SELECT CAST(2 AS CHAR)")))
		printf("[006] [%d] %s\n", $link->errno, $link->error);

	$column1 = null;
	if (!$stmt->execute() || !$stmt->bind_result($column1))
		printf("[007] [%d] %s\n", $stmt->errno, $stmt->error);

	$index = 0;
	while ($stmt->fetch()) {
		if ($data[$index] != $column1) {
			printf("[008] Row %d, expecting %s/%s got %s/%s\n",
				$index + 1, gettype($data[$index]), $data[$index], gettype($column1), $column1);
		}
		var_dump($column1);
		$index++;
	}
	$stmt->close();

	print "Using integer only...\n";
	if (!($res = $link->query("SELECT 1 AS column1 UNION SELECT 303 UNION SELECT 2")))
		printf("[009] [%d] %s\n", $link->errno, $link->error);

	$data = array();
	while ($row = $res->fetch_assoc()) {
		$data[] = $row['column1'];
	}
	$res->free();

	// Prepared Statements
	if (!($stmt = $link->prepare("SELECT 1 AS column1 UNION SELECT 303 UNION SELECT 2")))
		printf("[010] [%d] %s\n", $link->errno, $link->error);

	$column1 = null;
	if (!$stmt->bind_result($column1) || !$stmt->execute())
		printf("[011] [%d] %s\n", $stmt->errno, $stmt->error);

	$index = 0;
	while ($stmt->fetch()) {
		if ($data[$index] != $column1) {
			printf("[012] Row %d, expecting %s/%s got %s/%s\n",
				$index + 1, gettype($data[$index]), $data[$index], gettype($column1), $column1);
		}
		var_dump($column1);
		$index++;
	}
	$stmt->close();

	print "Testing bind_param(), strings only...\n";
	$two = 'two';
	$three = 'three';
	if (!($stmt = $link->prepare("SELECT 'one' AS column1 UNION SELECT ? UNION SELECT ?")))
		printf("[013] [%d] %s\n", $stmt->errno, $stmt->error);

	$column1 = null;
	if (!$stmt->bind_param('ss', $three, $two) || !$stmt->bind_result($column1) || !$stmt->execute())
		printf("[014] [%d] %s\n", $stmt->errno, $stmt->error);

	while ($stmt->fetch()) {
		var_dump($column1);
	}
	$stmt->close();

	print "Testing bind_param(), strings only, with CAST AS CHAR...\n";
	$two = 'two';
	$three = 'three beers are more than enough';
	if (!($stmt = $link->prepare("SELECT CAST('one' AS CHAR) AS column1 UNION SELECT CAST(? AS CHAR) UNION SELECT CAST(? AS CHAR)")))
		printf("[013] [%d] %s\n", $stmt->errno, $stmt->error);

	$column1 = null;
	if (!$stmt->bind_param('ss', $three, $two) || !$stmt->bind_result($column1) || !$stmt->execute())
		printf("[014] [%d] %s\n", $stmt->errno, $stmt->error);

	while ($stmt->fetch()) {
		var_dump($column1);
	}
	$stmt->close();

	$link->close();

	print "done!";
?>
--EXPECTF--
Using CAST('somestring' AS CHAR)...
string(3) "one"
string(5) "three"
string(3) "two"
Mixing CAST('somestring'AS CHAR), integer and CAST(integer AS CHAR)...
string(1) "1"
string(5) "three"
string(1) "2"
Using integer only...
int(1)
int(303)
int(2)
Testing bind_param(), strings only...
string(3) "one"
string(5) "three"
string(3) "two"
Testing bind_param(), strings only, with CAST AS CHAR...
string(3) "one"
string(32) "three beers are more than enough"
string(3) "two"
done!
