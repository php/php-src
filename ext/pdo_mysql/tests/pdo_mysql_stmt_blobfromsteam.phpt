--TEST--
MySQL PDOStatement - inserting BLOB from stream
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();

$tmp = MySQLPDOTest::getTempDir();
if (!$tmp)
	die("skip Can't create temporary file");

$file = $tmp . DIRECTORY_SEPARATOR . 'pdoblob.tst';
$fp = fopen($file, 'w');
if (!$fp)
	die("skip Can't create temporary file");

if (4 != fwrite($fp, 'test')) {
	die("skip Can't create temporary file");
}
fclose($fp);
clearstatcache();

if (!file_exists($file))
	die("skip Can't create temporary file");

unlink($file);
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	function blob_from_stream($offset, $db, $file, $blob) {

			@unlink($file);
			clearstatcache();
			if (file_exists($file)) {
				printf("[%03d + 1] Cannot remove old test file\n", $offset);
				return false;
			}

			$fp = fopen($file, 'w');
			if (!$fp || !fwrite($fp, $blob)) {
				printf("[%03d + 2] Cannot create test file '%s'\n", $offset, $file);
				return false;
			}

			fclose($fp);
			clearstatcache();
			if (!file_exists($file)) {
				printf("[%03d + 3] Failed to create test file '%s'\n", $offset, $file);
				return false;
			}

			$db->exec('DROP TABLE IF EXISTS test');
			$sql = sprintf('CREATE TABLE test(id INT, label BLOB) ENGINE=%s', PDO_MYSQL_TEST_ENGINE);
			$db->exec($sql);

			if (!$stmt = $db->prepare('INSERT INTO test(id, label) VALUES (?, ?)')) {
				printf("[%03d + 4] %s\n", $offset, var_export($db->errorInfo(), true));
				return false;
			}

			$fp = fopen($file, 'r');
			if (!$fp) {
				printf("[%03d + 5] Cannot create test file '%s'\n", $offset, $file);
				return false;
			}


			$id = 1;
			$stmt->bindParam(1, $id);
			if (true !== ($tmp = $stmt->bindParam(2, $fp, PDO::PARAM_LOB))) {
				printf("[%03d + 6] Expecting true, got %s. %s\n",
					$offset,
					var_export($tmp, true),
					var_export($db->errorInfo(), true));
				return false;
			}

			if (true !== $stmt->execute()) {
				printf("[%03d + 7] Failed to INSERT data, %s\n", $offset, var_export($stmt->errorInfo(), true));
				return false;
			}

			$stmt2 = $db->query('SELECT id, label FROM test WHERE id = 1');
			$row = $stmt2->fetch(PDO::FETCH_ASSOC);
			if ($row['label'] != $blob) {
				printf("[%03d + 8] INSERT and/or SELECT has failed, dumping data.\n", $offset);
				var_dump($row);
				var_dump($blob);
				return false;
			}

			// Lets test the chr(0) handling in case the streaming has failed:
			// is the bug about chr(0) or the streaming...
			$db->exec('DELETE FROM test');
			$stmt = $db->prepare('INSERT INTO test(id, label) VALUES (?, ?)');
			$stmt->bindParam(1, $id);
			$stmt->bindParam(2, $blob);
			if (true !== $stmt->execute())
				printf("[%03d + 9] %s\n", $offset, var_export($stmt->errorInfo(), true));

			$stmt2 = $db->query('SELECT id, label FROM test WHERE id = 1');
			$row = $stmt2->fetch(PDO::FETCH_ASSOC);
			if ($row['label'] != $blob) {
				printf("[%03d + 10] INSERT and/or SELECT has failed, dumping data.\n", $offset);
				var_dump($row);
				var_dump($blob);
				return false;
			}

			return true;
	}

	$db = MySQLPDOTest::factory();
	$blob = 'I am a mighty BLOB!' . chr(0) . "I am a binary thingie!";
	$tmp = MySQLPDOTest::getTempDir();
	$file = $tmp . DIRECTORY_SEPARATOR . 'pdoblob.tst';

	try {

		printf("Emulated PS...\n");
		$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
		blob_from_stream(10, $db, $file, $blob);

		printf("Native PS...\n");
		$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
		blob_from_stream(30, $db, $file, $blob);

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
@unlink(MySQLPDOTest::getTempDir() . DIRECTORY_SEPARATOR . 'pdoblob.tst');
?>
--EXPECT--
Emulated PS...
Native PS...
done!
