--TEST--
MySQL PDOStatement->fetch()
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();

	function fetch($offset, &$db, $query, $expect = null) {

		try {
			$stmt = $db->query('SELECT 1');
			$num = $stmt->fetch(PDO::FETCH_NUM);

			$stmt = $db->query('SELECT 1');
			$assoc = $stmt->fetch(PDO::FETCH_ASSOC);

			$stmt = $db->query('SELECT 1');
			$both = $stmt->fetch(PDO::FETCH_BOTH);

			$computed_both = array_merge($num, $assoc);
			if ($computed_both != $both) {
				printf("[%03d] Suspicious FETCH_BOTH result, dumping\n", $offset);
				var_dump($computed_both);
				var_dump($both);
			}

			if (!is_null($expect) && ($expect != $both)) {
				printf("[%03d] Expected differes from returned data, dumping\n", $offset);
				var_dump($expect);
				var_dump($both);
			}

		} catch (PDOException $e) {

			printf("[%03d] %s, [%s] %s\n",
				$offset,
				$e->getMessage(), $db->errroCode(), implode(' ', $db->errorInfo()));

		}

	}

	try {

		fetch(2, $db, 'SELECT 1', array(0 => '1', '1' => '1'));

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!";
?>
--EXPECTF--
[002] Suspicious FETCH_BOTH result, dumping
array(2) {
  [0]=>
  %unicode|string%(1) "1"
  [1]=>
  %unicode|string%(1) "1"
}
array(2) {
  [1]=>
  %unicode|string%(1) "1"
  [2]=>
  %unicode|string%(1) "1"
}
[002] Expected differes from returned data, dumping
array(2) {
  [0]=>
  %unicode|string%(1) "1"
  [1]=>
  %unicode|string%(1) "1"
}
array(2) {
  [1]=>
  %unicode|string%(1) "1"
  [2]=>
  %unicode|string%(1) "1"
}
done!