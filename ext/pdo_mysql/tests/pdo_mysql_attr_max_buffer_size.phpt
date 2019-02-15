--TEST--
MySQL PDO->__construct(), PDO::MYSQL_ATTR_MAX_BUFFER_SIZE
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
if (MySQLPDOTest::isPDOMySQLnd())
	die("skip PDO::MYSQL_ATTR_MAX_BUFFER_SIZE not supported with mysqlnd");
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	function try_buffer_size($offset, $buffer_size) {

		try {

			$dsn = MySQLPDOTest::getDSN();
			$user = PDO_MYSQL_TEST_USER;
			$pass = PDO_MYSQL_TEST_PASS;

			/* unsigned overflow possible ? */
			$db = new PDO($dsn, $user, $pass,
			array(
				PDO::MYSQL_ATTR_MAX_BUFFER_SIZE => $buffer_size,
				/* buffer is only relevant with native PS */
				PDO::MYSQL_ATTR_DIRECT_QUERY => 0,
				PDO::ATTR_EMULATE_PREPARES => 0,
			));

			$db->exec('DROP TABLE IF EXISTS test');
			$db->exec(sprintf('CREATE TABLE test(id INT, val LONGBLOB) ENGINE = %s', PDO_MYSQL_TEST_ENGINE));

			// 10 * (10 * 1024) = 10 * (10 * 1k) = 100k
			$db->exec('INSERT INTO test(id, val) VALUES (1, REPEAT("01234567890", 10240))');

			$stmt = $db->prepare('SELECT id, val FROM test');
			$stmt->execute();

			$id = $val = NULL;
			$stmt->bindColumn(1, $id);
			$stmt->bindColumn(2, $val);
			while ($row = $stmt->fetch(PDO::FETCH_BOUND)) {
				printf("[%03d] id = %d, val = %s... (length: %d)\n",
					$offset, $id, substr($val, 0, 10), strlen($val));
			}
			$db->exec('DROP TABLE IF EXISTS test');

		} catch (PDOException $e) {
			printf("[%03d] %s, [%s] %s\n",
				$offset,
				$e->getMessage(),
				(is_object($db)) ? $db->errorCode() : 'n/a',
				(is_object($db)) ? implode(' ', $db->errorInfo()) : 'n/a');
		}
	}

	try_buffer_size(1, -1);
	try_buffer_size(2, 1000);
	try_buffer_size(3, NULL);
	try_buffer_size(4, 2000);

	print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
[001] id = 1, val = 0123456789... (length: %d)
[002] id = 1, val = 0123456789... (length: 1000)
[003] id = 1, val = 0123456789... (length: %d)
[004] id = 1, val = 0123456789... (length: 2000)
done!
