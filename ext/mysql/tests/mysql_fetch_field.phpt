--TEST--
mysql_fetch_field()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	// Note: no SQL type tests, internally the same function gets used as for mysql_fetch_array() which does a lot of SQL type test
	if (!is_null($tmp = @mysql_fetch_field()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysql_fetch_field($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	$version = mysql_get_server_info($link);
	if (!preg_match('@(\d+)\.(\d+)\.(\d+)@ism', $version, $matches))
		printf("[003] Cannot get server version\n");
	$version = ($matches[1] * 100) + ($matches[2] * 10) + $matches[3];

	if (!$res = mysql_query("SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1", $link)) {
		printf("[004] [%d] %s\n", mysql_errno($link), mysql_error($link));
	}

	while ($tmp = mysql_fetch_field($res))
		var_dump($tmp);
	var_dump($tmp);

	mysql_free_result($res);

	if (!$res = mysql_query("SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 1", $link)) {
		printf("[005] [%d] %s\n", mysql_errno($link), mysql_error($link));
	}
	if (false !== ($tmp = mysql_fetch_field($res, PHP_INT_MAX - 1)))
		printf("[006] Expecting boolean/false got %s/%s\n", gettype($tmp), var_export($tmp, true));

	mysql_free_result($res);

	if (false !== ($tmp = mysql_fetch_field($res)))
		printf("[007] Expecting boolean/false, got %s/%s\n", gettype($tmp), var_export($tmp, true));

	$types = array(
		'BIT'               => array(1, 'int'),
		'TINYINT'           => array(1, 'int'),
		'BOOL'              => array('true', 'int'),
		'BOOL'              => array(1, 'int'),
		'SMALLINT'          => array(32767, 'int'),
		'MEDIUMINT'         => array(8388607, 'int'),
		'INT'               => array(100, 'int'),
		'BIGINT'            => array(100, 'int'),
		'FLOAT'             => array(100, 'real'),
		'DOUBLE'            => array(100, 'real'),
		'DECIMAL'           => array(100, 'real'),
		'DATE'              => array(@date('Y-m-d'), 'date'),
		'DATETIME'          => array(@date('Y-m-d H:i:s'), 'datetime'),
		'TIMESTAMP'         => array(@date('Y-m-d H:i:s'), 'timestamp'),
		'TIME'              => array(@date('H:i:s'), 'time'),
		'YEAR'              => array(@date('Y'), 'year'),
		'CHAR(1)'           => array('a', 'string'),
		'VARCHAR(1)'        => array('a', 'string'),
		'BINARY(1)'         => array('a', 'string'),
		'VARBINARY(1)'      => array('a', 'string'),
		'TINYBLOB'          => array('a', 'blob'),
		'TINYTEXT'          => array('a', 'blob'),
		'BLOB'              => array('a', 'blob'),
		'TEXT'              => array('a', 'blob'),
		'MEDIUMBLOB'        => array('a', 'blob'),
		'MEDIUMTEXT'        => array('a', 'blob'),
		'LONGBLOB'          => array('a', 'blob'),
		'LONGTEXT'          => array('a', 'blob'),
		'ENUM("a", "b")'    => array('a', 'string'), /* !!! */
		'SET("a", "b")'     => array('a', 'string'), /* !!! */
	);

	foreach ($types as $type_name => $type_desc) {
		if (!mysql_query("DROP TABLE IF EXISTS test", $link))
			printf("[008/%s] [%d] %s\n", $type_name, mysql_errno($link), mysql_error($link));
		if (!mysql_query(sprintf("CREATE TABLE test(id INT, label %s) ENGINE = %s", $type_name, $engine), $link)) {
			// server and/or engine might not support the data type
			continue;
		}

		if (is_string($type_desc[0]))
			$insert = sprintf("INSERT INTO test(id, label) VALUES (1, '%s')", $type_desc[0]);
		else
			$insert = sprintf("INSERT INTO test(id, label) VALUES (1, %s)", $type_desc[0]);

		if (!mysql_query($insert, $link)) {
			if (1366 == mysql_errno($link)) {
				/* Strict SQL mode - 1366, Incorrect integer value: 'true' for column 'label' at row 1 */
				continue;
			}
			printf("[009/%s] [%d] %s\n", $type_name, mysql_errno($link), mysql_error($link));
			continue;
		}
		if (!$res = mysql_query("SELECT id, label FROM test", $link)) {
			printf("[010/%s] [%d] %s\n", $type_name, mysql_errno($link), mysql_error($link));
			continue;
		}
		if (!$tmp = mysql_fetch_field($res, 1)) {
			printf("[011/%s] [%d] %s\n", $type_name, mysql_errno($link), mysql_error($link));
		}

		if ($type_desc[1] != $tmp->type) {
			printf("[012/%s] Expecting type '%s' got '%s'\n", $type_name, $type_desc[1], $tmp->type);
		}
		mysql_free_result($res);
	}

	if (!mysql_query("DROP TABLE IF EXISTS test", $link))
		printf("[013] [%d] %s\n", mysql_errno($link), mysql_error($link));

	if (!mysql_query("CREATE TABLE test(id INT DEFAULT 1)"))
		printf("[014] [%d] %s\n", mysql_errno($link), mysql_error($link));

	if (!mysql_query("INSERT INTO test(id) VALUES (2)"))
		printf("[015] [%d] %s\n", mysql_errno($link), mysql_error($link));

	if (!$res = mysql_query("SELECT id FROM test", $link)) {
		printf("[016] [%d] %s\n", mysql_errno($link), mysql_error($link));
	}

	var_dump(mysql_fetch_field($res));
	mysql_free_result($res);

	if (!$res = mysql_query("SELECT id FROM test", $link)) {
		printf("[017] [%d] %s\n", mysql_errno($link), mysql_error($link));
	}
	$res = mysql_list_fields($db, 'test');
	$found = false;
	while ($tmp = mysql_fetch_field($res)) {
		if ($tmp->name == 'id') {
			printf("Fetch field from mysql_list_fields result set.\n");
			$found = true;
			var_dump($tmp);
		}
	}
	if (!$found)
		printf("[018] mysqli_list_fields result set processing has failed.\n");

	mysql_free_result($res);

	mysql_close($link);
	print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
object(stdClass)#%d (13) {
  [%u|b%"name"]=>
  %unicode|string%(2) "ID"
  [%u|b%"table"]=>
  %unicode|string%(4) "TEST"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(1)
  [%u|b%"not_null"]=>
  int(1)
  [%u|b%"primary_key"]=>
  int(1)
  [%u|b%"multiple_key"]=>
  int(0)
  [%u|b%"unique_key"]=>
  int(0)
  [%u|b%"numeric"]=>
  int(1)
  [%u|b%"blob"]=>
  int(0)
  [%u|b%"type"]=>
  %unicode|string%(3) "int"
  [%u|b%"unsigned"]=>
  int(0)
  [%u|b%"zerofill"]=>
  int(0)
}
object(stdClass)#%d (13) {
  [%u|b%"name"]=>
  %unicode|string%(5) "label"
  [%u|b%"table"]=>
  %unicode|string%(4) "TEST"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(1)
  [%u|b%"not_null"]=>
  int(0)
  [%u|b%"primary_key"]=>
  int(0)
  [%u|b%"multiple_key"]=>
  int(0)
  [%u|b%"unique_key"]=>
  int(0)
  [%u|b%"numeric"]=>
  int(0)
  [%u|b%"blob"]=>
  int(0)
  [%u|b%"type"]=>
  %unicode|string%(6) "string"
  [%u|b%"unsigned"]=>
  int(0)
  [%u|b%"zerofill"]=>
  int(0)
}
bool(false)

Warning: mysql_fetch_field(): Bad field offset in %s on line %d

Warning: mysql_fetch_field(): supplied resource is not a valid MySQL result resource in %s on line %d
object(stdClass)#%d (13) {
  [%u|b%"name"]=>
  %unicode|string%(2) "id"
  [%u|b%"table"]=>
  %unicode|string%(4) "test"
  [%u|b%"def"]=>
  %unicode|string%(0) ""
  [%u|b%"max_length"]=>
  int(1)
  [%u|b%"not_null"]=>
  int(0)
  [%u|b%"primary_key"]=>
  int(0)
  [%u|b%"multiple_key"]=>
  int(0)
  [%u|b%"unique_key"]=>
  int(0)
  [%u|b%"numeric"]=>
  int(1)
  [%u|b%"blob"]=>
  int(0)
  [%u|b%"type"]=>
  %unicode|string%(3) "int"
  [%u|b%"unsigned"]=>
  int(0)
  [%u|b%"zerofill"]=>
  int(0)
}
Fetch field from mysql_list_fields result set.
object(stdClass)#%d (13) {
  [%u|b%"name"]=>
  %unicode|string%(2) "id"
  [%u|b%"table"]=>
  %unicode|string%(4) "test"
  [%u|b%"def"]=>
  %unicode|string%(1) "1"
  [%u|b%"max_length"]=>
  int(0)
  [%u|b%"not_null"]=>
  int(0)
  [%u|b%"primary_key"]=>
  int(0)
  [%u|b%"multiple_key"]=>
  int(0)
  [%u|b%"unique_key"]=>
  int(0)
  [%u|b%"numeric"]=>
  int(1)
  [%u|b%"blob"]=>
  int(0)
  [%u|b%"type"]=>
  %unicode|string%(3) "int"
  [%u|b%"unsigned"]=>
  int(0)
  [%u|b%"zerofill"]=>
  int(0)
}
done!
