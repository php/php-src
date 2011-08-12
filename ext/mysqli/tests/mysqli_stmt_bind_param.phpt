--TEST--
mysqli_stmt_bind_param()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	/*
	The way we test the INSERT and data types overlaps with
	the mysqli_stmt_bind_result test in large parts. There is only
	one difference. This test uses mysqli_query()/mysqli_fetch_assoc() to
	fetch the inserted values. This way we test
	mysqli_query()/mysqli_fetch_assoc() for all possible data types
	in this file and we test mysqli_stmt_bind_result() in the other
	test -- therefore the "duplicate" makes some sense to me.
	*/
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_stmt_bind_param()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_bind_param($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_bind_param($link, $link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	$stmt = mysqli_stmt_init($link);
	if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (?, ?)"))
		printf("[003] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = null;
	$label = null;

	/*
	libmysql gives a less descriptive error message but mysqlnd,
	we did not unify the error messages but ignore this slight difference silently
	*/
	if (!false === ($tmp = @mysqli_stmt_bind_param($stmt, " ", $tmp)))
		printf("[003d] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (!false === ($tmp = @mysqli_stmt_bind_param($stmt, "", $id, $label)))
		printf("[003a] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	/* TODO: somehwhat undocumented syntax! */
	$param = array($id);
	if (!false === ($tmp = mysqli_stmt_bind_param($stmt, "is", $param)))
		printf("[003b] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	$param = array($id, $label, $id);
	if (!false === ($tmp = mysqli_stmt_bind_param($stmt, "is", $param)))
		printf("[003c] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (!false === ($tmp = mysqli_stmt_bind_param($stmt, "a", $id)))
		printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (!false === ($tmp = mysqli_stmt_bind_param($stmt, "a", $id, $label)))
		printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (!false === ($tmp = mysqli_stmt_bind_param($stmt, "aa", $id, $label)))
		printf("[006] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (!false === ($tmp = mysqli_stmt_bind_param($stmt, "ia", $id, $label)))
		printf("[007] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (!true === ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label)))
		printf("[008] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (function_exists("memory_get_usage")) {
		$mem = memory_get_usage();
		for ($i = 0; $i < 20000; $i++) {
			if (!true === ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label)))
				printf("[008][$i] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
		}
		if (($tmp = (memory_get_usage() - $mem)) > 600)
			printf("[009] Function seems to be leaking, because it used %d bytes. During tests it used only 92 bytes.", $tmp);
	}

	$id = 100;
	$label = "z";
	if (!mysqli_stmt_execute($stmt))
		printf("[010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	mysqli_stmt_close($stmt);

	if (!($res = mysqli_query($link, "SELECT id, label FROM test WHERE id = " . $id)))
		printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$row = mysqli_fetch_assoc($res);
	if (($row['id'] != $id) || ($row['label'] != $label))
		printf("[012] Expecting '%s'/%s', got '%s'/%s'!\n", $id, $label, $row['id'], $row['label']);
	mysqli_free_result($res);

	function func_mysqli_stmt_bind_datatype($link, $engine, $bind_type, $sql_type, $bind_value, $offset, $alternative = null) {

		if (!mysqli_query($link, "DROP TABLE IF EXISTS test")) {
			printf("[%03d] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if (!mysqli_query($link, sprintf("CREATE TABLE test(id INT NOT NULL, label %s, PRIMARY KEY(id)) ENGINE = %s", $sql_type, $engine))) {
			// don't bail - it might be that the server does not support the data type
			return false;
		}

		if (!$stmt = mysqli_stmt_init($link)) {
			printf("[%03d] [%d] %s\n", $offset + 1, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUE (?, ?)")) {
			printf("[%03d] [%d] %s\n", $offset + 2, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		$id = 1;
		if (!mysqli_stmt_bind_param($stmt, "i" . $bind_type, $id, $bind_value)) {
			printf("[%03d] [%d] %s\n", $offset + 3, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if (!mysqli_stmt_execute($stmt)) {
			printf("[%03d] [%d] %s\n", $offset + 4, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}
		mysqli_stmt_close($stmt);

		if (!$res = mysqli_query($link, "SELECT id, label FROM test")) {
			printf("[%03d] [%d] %s\n", $offset + 5, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if (!$row = mysqli_fetch_assoc($res)) {
			printf("[%03d] [%d] %s\n", $offset + 5, mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if ($alternative) {
			if (($row['id'] != $id) || (($row['label'] != $bind_value) && ($row['label'] != $alternative))) {
				printf("[%03d] Testing '%s', '%s': expecting '%s'/'%s' (%s), got '%s'/'%s'\n",
					$offset + 6, $bind_type, $sql_type,
					$id, $bind_value, gettype($bind_value), $row['id'], $row['label']);
				return false;
			}
		} else {
			if (($row['id'] != $id) || ($row['label'] != $bind_value)) {
				printf("[%03d] Testing '%s', '%s': expecting '%s'/'%s', got '%s'/'%s'\n",
					$offset + 6, $bind_type, $sql_type,
					$id, $bind_value, $row['id'], $row['label']);
				return false;
			}
		}

		mysqli_free_result($res);
		return true;
	}

	function func_mysqli_stmt_bind_make_string($len) {

		$ret = '';
		for ($i = 0; $i < $len; $i++)
			$ret .= chr(mt_rand(65, 90));

		return $ret;
	}

	func_mysqli_stmt_bind_datatype($link, $engine, "i", "TINYINT", -11, 20);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "TINYINT", NULL, 30);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "TINYINT UNSIGNED", 1, 40);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "TINYINT UNSIGNED", NULL, 50);

	func_mysqli_stmt_bind_datatype($link, $engine, "i", "BOOL", 1, 60);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "BOOL", NULL, 70);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "BOOLEAN", 0, 80);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "BOOLEAN", NULL, 90);

	func_mysqli_stmt_bind_datatype($link, $engine, "i", "SMALLINT", -32768, 100);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "SMALLINT", 32767, 110);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "SMALLINT", NULL, 120);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "SMALLINT UNSIGNED", 65535, 130);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "SMALLINT UNSIGNED", NULL, 140);

	func_mysqli_stmt_bind_datatype($link, $engine, "i", "MEDIUMINT", -8388608, 150);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "MEDIUMINT", 8388607, 160);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "MEDIUMINT", NULL, 170);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "MEDIUMINT UNSIGNED", 16777215, 180);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "MEDIUMINT UNSIGNED", NULL, 190);

	func_mysqli_stmt_bind_datatype($link, $engine, "i", "INTEGER", -2147483648, 200);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "INTEGER", 2147483647, 210);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "INTEGER", NULL, 220);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "INTEGER UNSIGNED", (defined("PHP_INT_MAX")) ? min(4294967295, PHP_INT_MAX) : 1, 230);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "INTEGER UNSIGNED", 4294967295, 240);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "INTEGER UNSIGNED", NULL, 250);

	func_mysqli_stmt_bind_datatype($link, $engine, "i", "BIGINT", -1 * PHP_INT_MAX + 1, 260);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "BIGINT", NULL, 270);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "BIGINT", PHP_INT_MAX, 280);
	func_mysqli_stmt_bind_datatype($link, $engine, "i", "BIGINT UNSIGNED", NULL, 290);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "BIGINT", "-9223372036854775808", 900);
	// ?? func_mysqli_stmt_bind_datatype($link, $engine, "d", "BIGINT", -9223372036854775808, 910);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "BIGINT UNSIGNED", "18446744073709551615", 920);

/*
	??
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "FLOAT", -9223372036854775808 - 1.1, 300);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "FLOAT UNSIGNED", 18446744073709551615 + 1.1, 320);
	*/
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "FLOAT", NULL, 310);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "FLOAT UNSIGNED ", NULL, 330);
	if (2147483647 == PHP_INT_MAX) {
		func_mysqli_stmt_bind_datatype($link, $engine, "d", "FLOAT UNSIGNED", PHP_INT_MAX, 930, '2.14748e+09');
		func_mysqli_stmt_bind_datatype($link, $engine, "d", "FLOAT", -1 * PHP_INT_MAX + 1, 940, '-2.14748e+09');
	}
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "FLOAT", "-9223372036854775808", 300, '-9.22337e+18');
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "FLOAT UNSIGNED", "18446744073709551615", 320, '1.84467e+19');
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "FLOAT", -10.01, 950);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "FLOAT UNSIGNED", 10.01, 960);

	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DOUBLE(10,2)", NULL, 350);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DOUBLE(10,2) UNSIGNED", NULL, 370);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DOUBLE(10,2)", -99999999.99, 340);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DOUBLE(10,2) UNSIGNED", 99999999.99, 360);

	/*
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DOUBLE(10,2)", -99999999.99, 340);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DOUBLE(10,2) UNSIGNED", 99999999.99, 360);
		*/
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DECIMAL(10,2)", -99999999.99, 380);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DECIMAL(10,2)", NULL, 390);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DECIMAL(10,2)", 99999999.99, 400);
	func_mysqli_stmt_bind_datatype($link, $engine, "d", "DECIMAL(10,2)", NULL, 410);

	// don't care about date() strict TZ warnings...
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "DATE", @date('Y-m-d'), 420);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "DATE NOT NULL", @date('Y-m-d'), 430);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "DATE", NULL, 440);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "DATETIME", @date('Y-m-d H:i:s'), 450);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "DATETIME NOT NULL", @date('Y-m-d H:i:s'), 460);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "DATETIME", NULL, 470);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TIMESTAMP", @date('Y-m-d H:i:s'), 480);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TIME", @date('H:i:s'), 490);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TIME NOT NULL", @date('H:i:s'), 500);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TIME", NULL, 510);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "YEAR", @date('Y'), 520);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "YEAR NOT NULL", @date('Y'), 530);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "YEAR", NULL, 540);

	$string255 = func_mysqli_stmt_bind_make_string(255);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "CHAR(1)", "a", 550);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "CHAR(255)", $string255, 560);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "CHAR(1) NOT NULL", "a", 570);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "CHAR(1)", NULL, 580);

	$string65k = func_mysqli_stmt_bind_make_string(65535);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "VARCHAR(1)", "a", 590);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "VARCHAR(255)", $string255, 600);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "VARCHAR(65635)", $string65k, 610);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "VARCHAR(1) NOT NULL", "a", 620);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "VARCHAR(1)", NULL, 630);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "BINARY(1)", "a", 640);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "BINARY(1)", chr(0), 650);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "BINARY(1) NOT NULL", "b", 660);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "BINARY(1)", NULL, 670);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "VARBINARY(1)", "a", 680);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "VARBINARY(1)", chr(0), 690);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "VARBINARY(1) NOT NULL", "b", 700);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "VARBINARY(1)", NULL, 710);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TINYBLOB", "a", 720);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TINYBLOB", chr(0), 730);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TINYBLOB NOT NULL", "b", 740);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TINYBLOB", NULL, 750);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TINYTEXT", "a", 760);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TINYTEXT NOT NULL", "a", 770);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "TINYTEXT", NULL, 780);

	// Note: you cannot insert any blob values this way. But you can check the API at least partly this way
	// Extra BLOB tests are in mysqli_stmt_send_long()
	func_mysqli_stmt_bind_datatype($link, $engine, "b", "BLOB", "", 790);
	func_mysqli_stmt_bind_datatype($link, $engine, "b", "TEXT", "", 800);
	func_mysqli_stmt_bind_datatype($link, $engine, "b", "MEDIUMBLOB", "", 810);
	func_mysqli_stmt_bind_datatype($link, $engine, "b", "MEDIUMTEXT", "", 820);
	func_mysqli_stmt_bind_datatype($link, $engine, "b", "LONGBLOB", "", 830);
	func_mysqli_stmt_bind_datatype($link, $engine, "b", "LONGTEXT", "", 840);

	func_mysqli_stmt_bind_datatype($link, $engine, "s", "ENUM('a', 'b')", "a", 850);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "ENUM('a', 'b')", NULL, 860);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "SET('a', 'b')", "a", 870);
	func_mysqli_stmt_bind_datatype($link, $engine, "s", "SET('a', 'b')", NULL, 880);

	if (mysqli_get_server_version($link) >= 50600)
		func_mysqli_stmt_bind_datatype($link, $engine, "s", "TIME", "13:27:34.123456", 890, "13:27:34");

	$stmt = mysqli_stmt_init($link);
	if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (?, ?)"))
		printf("[2000] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = null;
	$label = null;
	if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label)))
		printf("[2001] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	mysqli_stmt_execute($stmt);

	if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label)))
		printf("[2002] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	mysqli_stmt_close($stmt);
	include("table.inc");

	if (!$stmt = mysqli_stmt_init($link))
		printf("[2003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (?, ?)"))
		printf("[2004] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = $label = null;
	if (true !== ($tmp = $stmt->bind_param('is', $id, $label)))
		printf("[2005] Expecting boolean/true got %s/%s, [%d] %s\n",
			gettype($tmp), $tmp,
			$stmt->errno, $stmt->error);

	$id = 100; $label = 'z';
	if (!$stmt->execute())
		printf("[2006] [%d] %s\n", $stmt->errno, $stmt->error);

	if (!$res = mysqli_query($link, "SELECT id, label FROM test WHERE id = 100"))
		printf("[2007] Expecting record 100/z, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[2008] Expecting row, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if ($row['id'] != 100  || $row['label'] != 'z') {
		printf("[2009] Row seems wrong, dumping record\n");
		var_dump($row);
	}
	mysqli_free_result($res);

	$value_list = array(array('id' => 101, 'label' => 'a'), array('id' => 102, 'label' => 'b'));
	if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (?, ?)"))
		printf("[2010] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

        foreach ($value_list as $k => $values) {
		if (!mysqli_stmt_bind_param($stmt, 'is', $values['id'], $values['label'])) {
			printf("[2011] bind_param() failed for id = %d, [%d] %s\n",
				$values['id'], mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			continue;
		}
		if (!$stmt->execute())
			printf("[2012] [%d] execute() failed for id = %d, [%d] %s\n",
				$values['id'], mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if (!$res = mysqli_query($link, sprintf("SELECT label FROM test WHERE id = %d", $values['id'])))
			printf("[2013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		if (!$row = mysqli_fetch_assoc($res))
			printf("[2014] Cannot find row id = %d\n", $values['id']);
		else if (isset($row['label']) && ($values['label'] != $row['label']))
			printf("[2015] Expecting label = %s, got label = %s\n", $values['label'], $row['label']);

		mysqli_free_result($res);
	}

	mysqli_stmt_close($stmt);
	mysqli_close($link);

	/* Check that the function alias exists. It's a deprecated function,
	but we have not announce the removal so far, therefore we need to check for it */
	if (!is_null($tmp = @mysqli_stmt_bind_param()))
			printf("[021] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_bind_param(): Number of elements in type definition string doesn't match number of bind variables in %s on line %d

Warning: mysqli_stmt_bind_param(): Number of elements in type definition string doesn't match number of bind variables in %s on line %d

Warning: mysqli_stmt_bind_param(): Number of variables doesn't match number of parameters in prepared statement in %s on line %d

Warning: mysqli_stmt_bind_param(): Number of elements in type definition string doesn't match number of bind variables in %s on line %d

Warning: mysqli_stmt_bind_param(): Undefined fieldtype a (parameter 3) in %s on line %d

Warning: mysqli_stmt_bind_param(): Undefined fieldtype a (parameter 4) in %s on line %d
done!
