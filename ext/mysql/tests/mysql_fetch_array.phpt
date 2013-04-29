--TEST--
mysql_fetch_array()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include_once "connect.inc";

$tmp    = NULL;
$link   = NULL;

if (NULL !== ($tmp = @mysql_fetch_array()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (NULL != ($tmp = @mysql_fetch_array($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');
if (!$res = mysql_query("SELECT * FROM test ORDER BY id LIMIT 5", $link)) {
	printf("[004] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

print "[005]\n";
var_dump(mysql_fetch_array($res));

print "[006]\n";
var_dump(mysql_fetch_array($res, MYSQL_NUM));

print "[007]\n";
var_dump(mysql_fetch_array($res, MYSQL_BOTH));

print "[008]\n";
var_dump(mysql_fetch_array($res, MYSQL_ASSOC));

print "[009]\n";
var_dump(mysql_fetch_array($res));

mysql_free_result($res);

if (!$res = mysql_query("SELECT 1 AS a, 2 AS a, 3 AS c, 4 AS C, NULL AS d, true AS e", $link)) {
	printf("[010] Cannot run query, [%d] %s\n", mysql_errno($link), mysql_error($link));
}
print "[011]\n";
var_dump(mysql_fetch_array($res, MYSQL_BOTH));

mysql_free_result($res);
if (!$res = mysql_query("SELECT 1 AS a, 2 AS b, 3 AS c, 4 AS C", $link)) {
	printf("[012] Cannot run query, [%d] %s\n",
		mysql_errno($link), $mysql_error($link));
	exit(1);
}

do {
	$illegal_mode = mt_rand(0, 10000);
} while (in_array($illegal_mode, array(MYSQL_ASSOC, MYSQL_NUM, MYSQL_BOTH)));
$tmp = mysql_fetch_array($res, $illegal_mode);
if (!is_array($tmp))
	printf("[013] Expecting array, got %s/%s. [%d] %s\n",
		gettype($tmp), $tmp, mysql_errno($link), mysql_error($link));

$tmp = @mysql_fetch_array($res, $illegal_mode);
if (false !== $tmp)
	printf("[014] Expecting boolean/false, got %s/%s. [%d] %s\n",
		gettype($tmp), $tmp, mysql_errno($link), mysql_error($link));

mysql_free_result($res);

function func_mysql_fetch_array($link, $engine, $sql_type, $sql_value, $php_value, $offset, $regexp_comparison = NULL, $binary_type = false) {

	if (!mysql_query("DROP TABLE IF EXISTS test", $link)) {
		printf("[%04d] [%d] %s\n", $offset, mysql_errno($link), mysql_error($link));
		return false;
	}

	if (!mysql_query($sql = sprintf("CREATE TABLE test(id INT NOT NULL, label %s, PRIMARY KEY(id)) ENGINE = %s", $sql_type, $engine), $link)) {
		// don't bail, engine might not support the datatype
		return false;
	}

	if (is_null($php_value) && !mysql_query($sql = sprintf("INSERT INTO test(id, label) VALUES (1, NULL)"), $link)) {
		printf("[%04d] [%d] %s\n", $offset + 1, mysql_errno($link), mysql_error($link));
		return false;
	}

	if (!is_null($php_value)) {
		if (is_int($sql_value) && !mysql_query(sprintf("INSERT INTO test(id, label) VALUES (1, '%d')", $sql_value), $link)) {
			printf("[%04d] [%d] %s\n", $offset + 1, mysql_errno($link), mysql_error($link));
			return false;
		} else if (!is_int($sql_value) && !mysql_query(sprintf("INSERT INTO test(id, label) VALUES (1, '%s')", $sql_value), $link)) {
			printf("[%04d] [%d] %s\n", $offset + 1, mysql_errno($link), mysql_error($link));
			return false;
		}
	}

	if (!$res = mysql_query("SELECT id, label FROM test", $link)) {
		printf("[%04d] [%d] %s\n", $offset + 2, mysql_errno($link), mysql_error($link));
		return false;
	}

	if (!$row = mysql_fetch_array($res, MYSQL_BOTH)) {
		printf("[%04d] [%d] %s\n", $offset + 3, mysql_errno($link), mysql_error($link));
		return false;
	}

	if ($regexp_comparison) {
		if (!preg_match($regexp_comparison, (string)$row['label']) || !preg_match($regexp_comparison, (string)$row[1])) {
		printf("[%04d] Expecting %s/%s [reg exp = %s], got %s/%s resp. %s/%s. [%d] %s\n", $offset + 4,
			gettype($php_value), $php_value, $regexp_comparison,
			gettype($row[1]), $row[1],
			gettype($row['label']), $row['label'], mysql_errno($link), mysql_error($link));
		return false;
		}
	} else if ((gettype($php_value) == 'unicode') && $binary_type) {
		// Unicode is on and we are told that the MySQL column type is a binary type.
		// Don't expect a unicode value from the database, you'll get binary string
		if (($row['label'] != $php_value) || ($row[1] != $php_value)) {
			printf("[%04d] Expecting %s/%s, got %s/%s resp. %s/%s. [%d] %s\n", $offset + 5,
				gettype($php_value), $php_value,
				gettype($row[1]), $row[1],
				gettype($row['label']), $row['label'], mysql_errno($link), mysql_error($link));
			return false;
		}
		if (gettype($row['label']) == 'unicode') {
			printf("[%04d] SQL Type: '%s', binary columns are supposed to return binary string and not unicode\n",
				$offset + 6, $sql_type);
			return false;
		}
	} else {
		if (($row['label'] !== $php_value) || ($row[1] != $php_value)) {
			printf("[%04d] Expecting %s/%s, got %s/%s resp. %s/%s. [%d] %s\n", $offset + 7,
				gettype($php_value), $php_value,
				gettype($row[1]), $row[1],
				gettype($row['label']), $row['label'], mysql_errno($link), mysql_error($link));
			return false;
		}
	}

	return true;
}

function func_mysql_fetch_array_make_string($len) {

	$ret = '';
	for ($i = 0; $i < $len; $i++)
		$ret .= chr(mt_rand(65, 90));

	return $ret;
}

func_mysql_fetch_array($link, $engine, "TINYINT", -11, "-11", 20);
func_mysql_fetch_array($link, $engine, "TINYINT", NULL, NULL, 30);
func_mysql_fetch_array($link, $engine, "TINYINT UNSIGNED", 1, "1", 40);
func_mysql_fetch_array($link, $engine, "TINYINT UNSIGNED", NULL, NULL, 50);

func_mysql_fetch_array($link, $engine, "BOOL", 1, "1", 60);
func_mysql_fetch_array($link, $engine, "BOOL", NULL, NULL, 70);
func_mysql_fetch_array($link, $engine, "BOOLEAN", 0, "0", 80);
func_mysql_fetch_array($link, $engine, "BOOLEAN", NULL, NULL, 90);

func_mysql_fetch_array($link, $engine, "SMALLINT", -32768, "-32768", 100);
func_mysql_fetch_array($link, $engine, "SMALLINT", 32767, "32767", 110);
func_mysql_fetch_array($link, $engine, "SMALLINT", NULL, NULL, 120);
func_mysql_fetch_array($link, $engine, "SMALLINT UNSIGNED", 65535, "65535", 130);
func_mysql_fetch_array($link, $engine, "SMALLINT UNSIGNED", NULL, NULL, 140);

func_mysql_fetch_array($link, $engine, "MEDIUMINT", -8388608, "-8388608", 150);
func_mysql_fetch_array($link, $engine, "MEDIUMINT", 8388607, "8388607", 160);
func_mysql_fetch_array($link, $engine, "MEDIUMINT", NULL, NULL, 170);
func_mysql_fetch_array($link, $engine, "MEDIUMINT UNSIGNED", 16777215, "16777215", 180);
func_mysql_fetch_array($link, $engine, "MEDIUMINT UNSIGNED", NULL, NULL, 190);

func_mysql_fetch_array($link, $engine, "INTEGER", -2147483648, "-2147483648", 200);
func_mysql_fetch_array($link, $engine, "INTEGER", 2147483647, "2147483647", 210);
func_mysql_fetch_array($link, $engine, "INTEGER", NULL, NULL, 220);
func_mysql_fetch_array($link, $engine, "INTEGER UNSIGNED", 4294967295, "4294967295", 230);
func_mysql_fetch_array($link, $engine, "INTEGER UNSIGNED", NULL, NULL, 240);

// func_mysql_fetch_array($link, $engine, "BIGINT", -9223372036854775808, "-9.22337e+018", 250, "/-9\.22337e\+[0]?18/iu");
func_mysql_fetch_array($link, $engine, "BIGINT", NULL, NULL, 260);
// func_mysql_fetch_array($link, $engine, "BIGINT UNSIGNED", 18446744073709551615, "1.84467e+019", 270, "/1\.84467e\+[0]?19/iu");
func_mysql_fetch_array($link, $engine, "BIGINT UNSIGNED", NULL, NULL, 280);

func_mysql_fetch_array($link, $engine, "FLOAT", -9223372036854775808 - 1.1, "-9.22337e+18", 290, "/-9\.22337e\+?[0]?18/iu");
func_mysql_fetch_array($link, $engine, "FLOAT", NULL, NULL, 300);
func_mysql_fetch_array($link, $engine, "FLOAT UNSIGNED", 18446744073709551615 + 1.1, "1.84467e+19", 310, "/1\.84467e\+?[0]?19/iu");
func_mysql_fetch_array($link, $engine, "FLOAT UNSIGNED ", NULL, NULL, 320);

func_mysql_fetch_array($link, $engine, "DOUBLE(10,2)", -99999999.99, "-99999999.99", 330);
func_mysql_fetch_array($link, $engine, "DOUBLE(10,2)", NULL, NULL, 340);
func_mysql_fetch_array($link, $engine, "DOUBLE(10,2) UNSIGNED", 99999999.99, "99999999.99", 350);
func_mysql_fetch_array($link, $engine, "DOUBLE(10,2) UNSIGNED", NULL, NULL, 360);

func_mysql_fetch_array($link, $engine, "DECIMAL(10,2)", -99999999.99, "-99999999.99", 370);
func_mysql_fetch_array($link, $engine, "DECIMAL(10,2)", NULL, NULL, 380);
func_mysql_fetch_array($link, $engine, "DECIMAL(10,2)", 99999999.99, "99999999.99", 390);
func_mysql_fetch_array($link, $engine, "DECIMAL(10,2)", NULL, NULL, 400);

// don't care about date() strict TZ warnings...
func_mysql_fetch_array($link, $engine, "DATE", @date('Y-m-d'), @date('Y-m-d'), 410);
func_mysql_fetch_array($link, $engine, "DATE NOT NULL", @date('Y-m-d'), @date('Y-m-d'), 420);
func_mysql_fetch_array($link, $engine, "DATE", NULL, NULL, 430);

func_mysql_fetch_array($link, $engine, "DATETIME", @date('Y-m-d H:i:s'), @date('Y-m-d H:i:s'), 440);
func_mysql_fetch_array($link, $engine, "DATETIME NOT NULL", @date('Y-m-d H:i:s'), @date('Y-m-d H:i:s'), 450);
func_mysql_fetch_array($link, $engine, "DATETIME", NULL, NULL, 460);

func_mysql_fetch_array($link, $engine, "TIMESTAMP", @date('Y-m-d H:i:s'), @date('Y-m-d H:i:s'), 470);

func_mysql_fetch_array($link, $engine, "TIME", @date('H:i:s'), @date('H:i:s'), 480);
func_mysql_fetch_array($link, $engine, "TIME NOT NULL", @date('H:i:s'), @date('H:i:s'), 490);
func_mysql_fetch_array($link, $engine, "TIME", NULL, NULL, 500);

func_mysql_fetch_array($link, $engine, "YEAR", @date('Y'), @date('Y'), 510);
func_mysql_fetch_array($link, $engine, "YEAR NOT NULL", @date('Y'), @date('Y'), 520);
func_mysql_fetch_array($link, $engine, "YEAR", NULL, NULL, 530);

$string255 = func_mysql_fetch_array_make_string(255);

func_mysql_fetch_array($link, $engine, "CHAR(1)", "a", "a", 540);
func_mysql_fetch_array($link, $engine, "CHAR(255)", $string255,  $string255, 550);
func_mysql_fetch_array($link, $engine, "CHAR(1) NOT NULL", "a", "a", 560);
func_mysql_fetch_array($link, $engine, "CHAR(1)", NULL, NULL, 570);

$string65k = func_mysql_fetch_array_make_string(65400);

func_mysql_fetch_array($link, $engine, "VARCHAR(1)", "a", "a", 580);
func_mysql_fetch_array($link, $engine, "VARCHAR(255)", $string255, $string255, 590);
func_mysql_fetch_array($link, $engine, "VARCHAR(65400)", $string65k, $string65k, 600);
func_mysql_fetch_array($link, $engine, "VARCHAR(1) NOT NULL", "a", "a", 610);
func_mysql_fetch_array($link, $engine, "VARCHAR(1)", NULL, NULL, 620);

func_mysql_fetch_array($link, $engine, "BINARY(1)", "a", "a", 630, null , true);
func_mysql_fetch_array($link, $engine, "BINARY(1) NOT NULL", "b", "b", 650, null , true);
func_mysql_fetch_array($link, $engine, "BINARY(1)", NULL, NULL, 660, null , true);

func_mysql_fetch_array($link, $engine, "VARBINARY(1)", "a", "a", 670, null , true);
func_mysql_fetch_array($link, $engine, "VARBINARY(1) NOT NULL", "b", "b", 690, null , true);
func_mysql_fetch_array($link, $engine, "VARBINARY(1)", NULL, NULL, 700, null , true);

func_mysql_fetch_array($link, $engine, "TINYBLOB", "a", "a", 710, null , true);
func_mysql_fetch_array($link, $engine, "TINYBLOB NOT NULL", "b", "b", 730, null , true);
func_mysql_fetch_array($link, $engine, "TINYBLOB", NULL, NULL, 740, null , true);

func_mysql_fetch_array($link, $engine, "TINYTEXT", "a", "a", 750);
func_mysql_fetch_array($link, $engine, "TINYTEXT NOT NULL", "a", "a", 760);
func_mysql_fetch_array($link, $engine, "TINYTEXT", NULL, NULL, 770);

func_mysql_fetch_array($link, $engine, "BLOB", "a", "a", 780, null , true);
func_mysql_fetch_array($link, $engine, "BLOB", NULL, NULL, 790, null , true);

func_mysql_fetch_array($link, $engine, "TEXT", "a", "a", 800);
func_mysql_fetch_array($link, $engine, "TEXT", NULL, NULL, 820);

func_mysql_fetch_array($link, $engine, "MEDIUMBLOB", "a", "a", 830, null , true);
func_mysql_fetch_array($link, $engine, "MEDIUMBLOB", NULL, NULL, 850, null , true);

func_mysql_fetch_array($link, $engine, "MEDIUMTEXT", "a", "a", 860);
func_mysql_fetch_array($link, $engine, "MEDIUMTEXT", NULL, NULL, 880);

func_mysql_fetch_array($link, $engine, "LONGBLOB", "a", "a", 890, null , true);
func_mysql_fetch_array($link, $engine, "LONGBLOB", NULL, NULL, 910, null , true);

func_mysql_fetch_array($link, $engine, "ENUM('a', 'b')", "a", "a", 920);
func_mysql_fetch_array($link, $engine, "ENUM('a', 'b')", NULL, NULL, 930);

func_mysql_fetch_array($link, $engine, "SET('a', 'b')", "a", "a", 940);
func_mysql_fetch_array($link, $engine, "SET('a', 'b')", NULL, NULL, 950);

mysql_close($link);

if (false !== ($tmp = mysql_fetch_array($res, MYSQL_ASSOC)))
printf("[015] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
[005]
array(4) {
  [0]=>
  %unicode|string%(1) "1"
  [%u|b%"id"]=>
  %unicode|string%(1) "1"
  [1]=>
  %unicode|string%(1) "a"
  [%u|b%"label"]=>
  %unicode|string%(1) "a"
}
[006]
array(2) {
  [0]=>
  %unicode|string%(1) "2"
  [1]=>
  %unicode|string%(1) "b"
}
[007]
array(4) {
  [0]=>
  %unicode|string%(1) "3"
  [%u|b%"id"]=>
  %unicode|string%(1) "3"
  [1]=>
  %unicode|string%(1) "c"
  [%u|b%"label"]=>
  %unicode|string%(1) "c"
}
[008]
array(2) {
  [%u|b%"id"]=>
  %unicode|string%(1) "4"
  [%u|b%"label"]=>
  %unicode|string%(1) "d"
}
[009]
array(4) {
  [0]=>
  %unicode|string%(1) "5"
  [%u|b%"id"]=>
  %unicode|string%(1) "5"
  [1]=>
  %unicode|string%(1) "e"
  [%u|b%"label"]=>
  %unicode|string%(1) "e"
}
[011]
array(11) {
  [0]=>
  %unicode|string%(1) "1"
  [%u|b%"a"]=>
  %unicode|string%(1) "2"
  [1]=>
  %unicode|string%(1) "2"
  [2]=>
  %unicode|string%(1) "3"
  [%u|b%"c"]=>
  %unicode|string%(1) "3"
  [3]=>
  %unicode|string%(1) "4"
  [%u|b%"C"]=>
  %unicode|string%(1) "4"
  [4]=>
  NULL
  [%u|b%"d"]=>
  NULL
  [5]=>
  %unicode|string%(1) "1"
  [%u|b%"e"]=>
  %unicode|string%(1) "1"
}

Warning: mysql_fetch_array(): The result type should be either MYSQL_NUM, MYSQL_ASSOC or MYSQL_BOTH in %s on line %d

Warning: mysql_fetch_array(): %d is not a valid MySQL result resource in %s on line %d
done!
