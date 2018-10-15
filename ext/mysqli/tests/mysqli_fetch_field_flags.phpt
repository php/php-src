--TEST--
mysqli_fetch_field() - flags/field->flags
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

require_once('connect.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		die(printf("skip: [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error()));

if (mysqli_get_server_version($link) < 50041)
	die("skip: Due to many MySQL Server differences, the test requires 5.0.41+");

mysqli_close($link);
?>
--FILE--
<?php
	require_once("connect.inc");

/* TODO: mysqli.c needs to export a few more constants - see all the defined() calls! */

	$flags = array(
		MYSQLI_NOT_NULL_FLAG => 'NOT_NULL',
		MYSQLI_PRI_KEY_FLAG => 'PRI_KEY',
		MYSQLI_UNIQUE_KEY_FLAG => 'UNIQUE_KEY',
		MYSQLI_MULTIPLE_KEY_FLAG => 'MULTIPLE_KEY',
		MYSQLI_BLOB_FLAG => 'BLOB',
		MYSQLI_UNSIGNED_FLAG	=> 'UNSIGNED',
		MYSQLI_ZEROFILL_FLAG => 'ZEROFILL',
		MYSQLI_AUTO_INCREMENT_FLAG => 'AUTO_INCREMENT',
		MYSQLI_TIMESTAMP_FLAG	=> 'TIMESTAMP',
		MYSQLI_SET_FLAG	=> 'SET',
		MYSQLI_NUM_FLAG => 'NUM',
		MYSQLI_PART_KEY_FLAG => 'PART_KEY',
		// MYSQLI_GROUP_FLAG => 'MYSQLI_GROUP_FLAG' - internal usage only
		(defined('MYSQLI_NO_DEFAULT_VALUE_FLAG') ? MYSQLI_NO_DEFAULT_VALUE_FLAG : 4096) => 'NO_DEFAULT_VALUE',
		(defined('MYSQLI_BINARY_FLAG') ? MYSQLI_BINARY_FLAG : 128) => 'BINARY',
		(defined('MYSQLI_ENUM_FLAG') ? MYSQLI_ENUM_FLAG : 256) => 'ENUM',
		// MYSQLI_BINCMP_FLAG
	);

	// 5.1.24 / 6.0.4+
	if (defined('MYSQLI_ON_UPDATE_NOW'))
		$flags[MYSQLI_ON_UPDATE_NOW] = 'ON_UPDATE_NOW';
	else
		$flags[8192] = 'ON_UPDATE_NOW';

	krsort($flags);

	$columns = array(
		'INT DEFAULT NULL' => 'NUM',
		'INT NOT NULL' => 'NOT_NULL NO_DEFAULT_VALUE NUM',
		'INT NOT NULL DEFAULT 1' => 'NOT_NULL NUM',
		'INT UNSIGNED DEFAULT NULL' => 'UNSIGNED NUM',
		'INT UNSIGNED NOT NULL'	=> 'NOT_NULL UNSIGNED NO_DEFAULT_VALUE NUM',
		'INT UNSIGNED NOT NULL DEFAULT 1' => 'NOT_NULL UNSIGNED NUM',
		'INT UNSIGNED ZEROFILL DEFAULT NULL' => 'UNSIGNED ZEROFILL NUM',
		'INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY' => 'NOT_NULL PRI_KEY UNSIGNED AUTO_INCREMENT NUM PART_KEY',
		'CHAR(1) DEFAULT NULL'	=> '',
		'CHAR(1) NOT NULL' => 'NOT_NULL NO_DEFAULT_VALUE',
		'TIMESTAMP NOT NULL' => 'NOT_NULL UNSIGNED ZEROFILL BINARY TIMESTAMP',
		'VARBINARY(127) DEFAULT NULL' => 'BINARY',
		'BLOB'	=> 'BLOB BINARY',
		'TINYBLOB'	=> 'BLOB BINARY',
		'MEDIUMBLOB'	=> 'BLOB BINARY',
		'LONGBLOB'	=> 'BLOB BINARY',
		'TEXT'	=> 'BLOB',
		'TINYTEXT'	=> 'BLOB',
		'MEDIUMTEXT'	=> 'BLOB',
		'LONGTEXT'	=> 'BLOB',
		'SET("one", "two")'	=> 'SET',
		'SET("one", "two") NOT NULL'	=> 'NOT_NULL SET NO_DEFAULT_VALUE',
		'SET("one", "two") NOT NULL DEFAULT "one"'	=> 'NOT_NULL SET',
		'ENUM("one", "two")'	=> 'ENUM',
		'ENUM("one", "two") NOT NULL' => 'NOT_NULL ENUM NO_DEFAULT_VALUE',
		'ENUM("one", "two") NOT NULL DEFAULT "one"' => 'NOT_NULL ENUM',
		'TINYINT UNIQUE' => 'UNIQUE_KEY NUM PART_KEY',
		'SMALLINT UNIQUE' => 'UNIQUE_KEY NUM PART_KEY',
		'MEDIUMINT UNIQUE DEFAULT 1' => 'UNIQUE_KEY NUM PART_KEY',
		'BIGINT UNSIGNED UNIQUE DEFAULT 100' => 'UNIQUE_KEY UNSIGNED NUM PART_KEY',
		'BIT' => 'UNSIGNED',
		'VARCHAR(2) NOT NULL PRIMARY KEY' => 'NOT_NULL PRI_KEY NO_DEFAULT_VALUE PART_KEY'
	);



	function checkFlags($reported_flags, $expected_flags, $flags) {
		$found_flags = $unexpected_flags = '';
		foreach ($flags as $code => $name) {
			if ($reported_flags >= $code) {
				$reported_flags -= $code;
				$found_flags .= $name . ' ';
				if (stristr($expected_flags, $name)) {
					$expected_flags = trim(str_ireplace($name, '', $expected_flags));
				} else {
					$unexpected_flags .= $name . ' ';
				}
			}
		}

		return array($expected_flags, $unexpected_flags, $found_flags);
	}

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	$is_maria_db = strpos(mysqli_get_server_info($link), "MariaDB") !== false;
	if ($is_maria_db) {
		$columns['TIMESTAMP NOT NULL'] = 'ON_UPDATE_NOW TIMESTAMP BINARY UNSIGNED NOT_NULL';
	} else if (mysqli_get_server_version($link) > 50600) {
		$columns['TIMESTAMP NOT NULL'] = 'ON_UPDATE_NOW TIMESTAMP BINARY NOT_NULL';
	}

	foreach ($columns as $column_def => $expected_flags) {
		if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
			printf("[002] %s [%d] %s\n", $column_def,
				mysqli_errno($link), mysqli_error($link));
			continue;
		}

		$create = sprintf('CREATE TABLE test(id INT, col1 %s)', $column_def);
		if (!mysqli_query($link, $create)) {
			// Server might not support it - skip
			continue;
		}

		if (!$res = mysqli_query($link, 'SELECT * FROM test')) {
			printf("[003] Can't select from table, %s [%d] %s\n", $column_def,
				mysqli_errno($link), mysqli_error($link));
			continue;
		}

		$field = mysqli_fetch_field_direct($res, 1);
		if (!is_object($field)) {
			printf("[004] Fetching the meta data failed, %s [%d] %s\n", $column_def,
				mysqli_errno($link), mysqli_error($link));
			continue;
		}
		if ($field->name != 'col1') {
			printf("[005] Field information seems wrong, %s [%d] %s\n", $column_def,
				mysqli_errno($link), mysqli_error($link));
			continue;
		}

		/*
		TODO
		Unfortunately different server versions give you slightly different
		results.The test does not yet fully reflect all server changes/bugs etc.
		*/
		switch ($column_def) {
			case 'TIMESTAMP NOT NULL':
				// http://bugs.mysql.com/bug.php?id=30081 - new flag introduced in 5.1.24/6.0.4
				$version = mysqli_get_server_version($link);
				if ((($version >  50122) && ($version < 60000) && ($version != 50200)) ||
						($version >= 60004)) {
					// new flag ON_UPDATE_NOW_FLAG (8192)
					$expected_flags .= ' ON_UPDATE_NOW';
				}
				break;

			case 'INT UNSIGNED NOT NULL':
			case 'INT NOT NULL':
			case 'CHAR(1) NOT NULL':
			case 'SET("one", "two") NOT NULL':
			case 'ENUM("one", "two") NOT NULL':
				$version = mysqli_get_server_version($link);
				if ($version < 50000) {
					// TODO - check exact version!
					$expected_flags = trim(str_replace('NO_DEFAULT_VALUE', '', $expected_flags));
				}
				break;

			case 'BIT':
				$version = mysqli_get_server_version($link);
				if (($version <= 50114 && $version > 50100) || ($version == 50200)) {
					// TODO - check exact version!
					$expected_flags = trim(str_replace('UNSIGNED', '', $expected_flags));
				}

			default:
				break;
		}

		list($missing_flags, $unexpected_flags, $flags_found) = checkFlags($field->flags, $expected_flags, $flags);
		if ($unexpected_flags) {
			printf("[006] Found unexpected flags '%s' for %s, found '%s' with MySQL %s'\n",
				$unexpected_flags, $column_def, $flags_found, mysqli_get_server_version($link));
		}
		if ($missing_flags) {
			printf("[007] The flags '%s' have not been reported for %s, found '%s'\n",
				$missing_flags, $column_def, $flags_found);
			var_dump($create);
			var_dump(mysqli_get_server_version($link));
			die($missing_flags);
		}

		mysqli_free_result($res);
	}

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
		printf("[008] %s [%d] %s\n", $column_def,
			mysqli_errno($link), mysqli_error($link));
	}

	$column_def = array('col1 CHAR(1)', 'col2 CHAR(2)','INDEX idx_col1_col2(col1, col2)');
	$expected_flags = array('col1' => 'MULTIPLE_KEY PART_KEY', 'col2' => 'PART_KEY');
	$create = 'CREATE TABLE test(id INT, ';
	foreach ($column_def as $k => $v) {
		$create .= sprintf('%s, ', $v);
	}
	$create = sprintf('%s)', substr($create, 0, -2));

	if (mysqli_query($link, $create)) {
		if (!$res = mysqli_query($link, 'SELECT * FROM test')) {
			printf("[009] Cannot run SELECT, [%d] %s\n",
				mysqli_errno($link), mysqli_error($link));
		}
		// id column - skip it
		$field = mysqli_fetch_field($res);
		while ($field = mysqli_fetch_field($res)) {
			if (!isset($expected_flags[$field->name])) {
				printf("[010] Found unexpected field '%s'\n", $field->name);
			}
			list($missing_flags, $unexpected_flags, $flags_found) = checkFlags($field->flags, $expected_flags[$field->name], $flags);
			if ($unexpected_flags)
				printf("[011] Found unexpected flags '%s' for %s, found '%s'\n",
					$unexpected_flags, $field->name, $flags_found);
			if ($missing_flags)
				printf("[012] The flags '%s' have not been reported for %s, found '%s'\n",
					$missing_flags, $field->name, $flags_found);
		}
	}

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!
