--TEST--
mysqli_stmt_result_metadata() - non SELECT statements
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

die("skip Check again when the Klingons visit earth - http://bugs.mysql.com/bug.php?id=42490");
?>
--FILE--
<?php
	require('table.inc');

	function testStatement($offset, $link, $sql, $expected_lib, $expected_mysqlnd, $check_mysqlnd, $compare) {

		if (!$stmt = mysqli_stmt_init($link)) {
			printf("[%04d - %s] [%d] %s\n",
				$offset, $sql,
				mysqli_errno($link), mysqli_error($link));
			return false;
		}

		if (!@mysqli_stmt_prepare($stmt, $sql)) {
			/* Not all server versions will support all statements */
			/* Failing to prepare is OK */
			return true;
		}

		if (empty($expected_lib) && (false !== $res)) {
			printf("[%04d - %s] No metadata expected\n",
				$offset + 1, $sql);
			return false;
		} else if (!empty($expected_lib) && (false == $res)) {
			printf("[%04d - %s] Metadata expected\n",
				$offset + 2, $sql);
			return false;
		}
		if (!empty($expected_lib)) {
			if (!is_object($res)) {
				printf("[%04d - %s] [%d] %s\n",
					$offset + 3, $sql,
					mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
				return false;
			}
			if (get_class($res) != 'mysqli_result') {
				printf("[%04d - %s] Expecting object/mysqli_result got object/%s\n",
					$offset + 4, $sql, get_class($res));
				return false;
			}

			$meta = array(
				'num_fields'		=> mysqli_num_fields($res),
				'fetch_field'		=> mysqli_fetch_field($res),
				'fetch_field_direct0'	=> mysqli_fetch_field_direct($res, 0),
				'fetch_field_direct1'	=> @mysqli_fetch_field_direct($res, 1),
				'fetch_fields'		=> count(mysqli_fetch_fields($res)),
				'field_count'		=> $res->field_count,
				'field_seek-1'		=> @mysqli_field_seek($res, -1),
				'field_seek0'		=> mysqli_field_seek($res, 0),
				'field_tell'		=> mysqli_field_tell($res),
			);
			if (is_object($meta['fetch_field'])) {
				$meta['fetch_field']->charsetnr	= 'ignore';
				$meta['fetch_field']->flags	= 'ignore';
			}
			if (is_object($meta['fetch_field_direct0'])) {
				$meta['fetch_field_direct0']->charsetnr	= 'ignore';
				$meta['fetch_field_direct0']->flags	= 'ignore';
			}
			if (is_object($meta['fetch_field_direct1'])) {
				$meta['fetch_field_direct1']->charsetnr	= 'ignore';
				$meta['fetch_field_direct1']->flags	= 'ignore';
			}
			mysqli_free_result($res);

			if ($meta != $expected_lib) {
				printf("[%04d - %s] Metadata differes from expected values\n",
					$offset + 5, $sql);
				var_dump($meta);
				var_dump($expected_lib);
				return false;
			}
		}

		if (function_exists('mysqli_stmt_get_result')) {
			/* mysqlnd only */
			if (!mysqli_stmt_execute($stmt)) {
				printf("[%04d - %s] [%d] %s\n",
					$offset + 6, $sql,
					mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
				return false;
			}

			$res = mysqli_stmt_get_result($stmt);
			if (false === $res && !empty($expected_mysqlnd)) {
				printf("[%04d - %s] Expecting resultset [%d] %s\n",
					$offset + 7, $sql,
					mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
				return false;
			} else if (empty($expected_mysqlnd) && false !== $res) {
				printf("[%04d - %s] Unexpected resultset [%d] %s\n",
					$offset + 8, $sql,
					mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
				return false;
			}

			if (!is_object($res)) {
				printf("[%04d - %s] [%d] %s\n",
					$offset + 9, $sql,
					mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
				return false;
			}
			if ('mysqli_result' != get_class($res)) {
				printf("[%04d - %s] Expecting object/mysqli_result got object/%s\n",
					$offset + 10, $sql,
					get_class($res));
				return false;
			}

			$meta_res = array(
				'num_fields'		=> mysqli_num_fields($res),
				'fetch_field'		=> mysqli_fetch_field($res),
				'fetch_field_direct0'	=> mysqli_fetch_field_direct($res, 0),
				'fetch_field_direct1'	=> @mysqli_fetch_field_direct($res, 1),
				'fetch_fields'		=> count(mysqli_fetch_fields($res)),
				'field_count'		=> mysqli_field_count($link),
				'field_seek-1'		=> @mysqli_field_seek($res, -1),
				'field_seek0'		=> mysqli_field_seek($res, 0),
				'field_tell'		=> mysqli_field_tell($res),
			);
			if (is_object($meta_res['fetch_field'])) {
				$meta_res['fetch_field']->charsetnr	= 'ignore';
				$meta_res['fetch_field']->flags	= 'ignore';
			}
			if (is_object($meta_res['fetch_field_direct0'])) {
				$meta_res['fetch_field_direct0']->charsetnr	= 'ignore';
				$meta_res['fetch_field_direct0']->flags	= 'ignore';
			}
			if (is_object($meta_res['fetch_field_direct1'])) {
				$meta_res['fetch_field_direct1']->charsetnr	= 'ignore';
				$meta_res['fetch_field_direct1']->flags	= 'ignore';
			}
			mysqli_free_result($res);
			if ($check_mysqlnd && $meta_res != $expected_mysqlnd) {
				printf("[%04d - %s] Metadata differs from expected\n",
					$offset + 11, $sql);
				var_dump($meta_res);
				var_dump($expected_mysqlnd);
			} else {
				if ($meta_res['field_count'] < 1) {
					printf("[%04d - %s] Metadata seems wrong, no fields?\n",
					$offset + 12, $sql);
					var_dump($meta_res);
					var_dump(mysqli_fetch_assoc($res));
				}
			}

			if ($compare && $meta_res != $meta) {
				printf("[%04d - %s] Metadata returned by mysqli_stmt_result_metadata() and mysqli_stmt_get_result() differ\n",
					$offset + 13, $sql);
				var_dump($meta_res);
				var_dump($meta);
			}

		}

		mysqli_stmt_close($stmt);
		return true;
	}

	/* Note: very weak testing, we accept almost any result */

	testStatement(100, $link, 'ANALYZE TABLE test', array(), array(1), false, false);
	testStatement(120, $link, 'OPTIMIZE TABLE test', array(), array(1), false, false);
	testStatement(140, $link, 'REPAIR TABLE test', array(), array(1), false, false);

	testStatement(160, $link, 'SHOW AUTHORS', array(), array(1), false, false);
	testStatement(180, $link, 'SHOW CHARACTER SET', array(), array(1), false, false);
	testStatement(200, $link, 'SHOW COLLATION', array(), array(1), false, false);
	testStatement(220, $link, 'SHOW CONTRIBUTORS', array(), array(1), false, false);
	testStatement(240, $link, 'SHOW CREATE DATABASE ' . $db, array(), array(1), false, false);
	testStatement(260, $link, 'SHOW DATABASES', array(), array(1), false, false);
	testStatement(280, $link, 'SHOW ENGINE InnoDB STATUS', array(), array(1), false, false);
	testStatement(300, $link, 'SHOW ENGINES', array(), array(1), false, false);
	testStatement(320, $link, 'SHOW PLUGINS', array(), array(1), false, false);
	testStatement(340, $link, 'SHOW PROCESSLIST', array(), array(1), false, false);
	testStatement(360, $link, 'SHOW FULL PROCESSLIST', array(), array(1), false, false);
	testStatement(380, $link, 'SHOW STATUS', array(), array(1), false, false);
	testStatement(400, $link, 'SHOW TABLE STATUS', array(), array(1), false, false);
	testStatement(420, $link, 'SHOW TABLE STATUS', array(), array(1), false, false);
	testStatement(440, $link, 'SHOW TABLES', array(), array(1), false, false);
	testStatement(460, $link, 'SHOW OPEN TABLES', array(), array(1), false, false);
	testStatement(460, $link, 'SHOW VARIABLES', array(), array(1), false, false);

	$field0 = new stdClass();
	$field0->name 		= 'id';
	$field0->orgname 	= 'id';
	$field0->table		= 'test';
	$field0->orgtable	= 'test';
	$field0->def 		= '';
	$field0->max_length	= 0;
	$field0->length		= 11;
	$field0->charsetnr	= 'ignore';
	$field0->flags		= 'ignore';
	$field0->type		= MYSQLI_TYPE_LONG;
	$field0->decimals	= 0;
	$meta_lib = array(
		'num_fields'		=> 1,
		'fetch_field'		=> $field0,
		'fetch_field_direct0'	=> $field0,
		'fetch_field_direct1'	=> false,
		'fetch_fields'		=> 1,
		'field_count'		=> 1,
		'field_seek-1'		=> false,
		'field_seek0'		=> true,
		'field_tell'		=> 0,
	);
	$meta_mysqlnd = $meta_lib;
	testStatement(480, $link, 'SELECT id FROM test', $meta_lib, $meta_mysqlnd, true, true);

	testStatement(500, $link, 'CHECKSUM TABLE test', array(), array(1), false, false);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!
