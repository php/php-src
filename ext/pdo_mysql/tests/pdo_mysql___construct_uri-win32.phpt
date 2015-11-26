--TEST--
MySQL PDO->__construct() - URI
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	try {

		if ($tmp = MySQLPDOTest::getTempDir()) {

			$file = $tmp . DIRECTORY_SEPARATOR . 'pdomuri.tst';
			$dsn = MySQLPDOTest::getDSN();
			$user = PDO_MYSQL_TEST_USER;
			$pass = PDO_MYSQL_TEST_PASS;
			$uri = sprintf('uri:file:%s', $file);

			if ($fp = @fopen($file, 'w')) {
				// ok, great we can create a file with a DSN in it
				fwrite($fp, $dsn);
				fclose($fp);
				clearstatcache();
				assert(file_exists($file));
				try {
					$db = new PDO($uri, $user, $pass);
				} catch (PDOException $e) {
					printf("[002] URI=%s, DSN=%s, File=%s (%d bytes, '%s'), %s\n",
						$uri, $dsn,
						$file, filesize($file), file_get_contents($file),
						$e->getMessage());
				}
				unlink($file);
			}

			if ($fp = @fopen($file, 'w')) {
				fwrite($fp, sprintf('mysql:dbname=letshopeinvalid;%s%s',
					chr(0), $dsn));
				fclose($fp);
				clearstatcache();
				assert(file_exists($file));
				try {
					$db = new PDO($uri, $user, $pass);
				} catch (PDOException $e) {
					printf("[003] URI=%s, DSN=%s, File=%s (%d bytes, '%s'), chr(0) test, %s\n",
					$uri, $dsn,
					$file, filesize($file), file_get_contents($file),
					$e->getMessage());
				}
				unlink($file);
			}

		}

		/* TODO: safe mode */

	} catch (PDOException $e) {
		printf("[001] %s, [%s] %s\n",
			$e->getMessage(),
			(is_object($db)) ? $db->errorCode() : 'n/a',
			(is_object($db)) ? implode(' ', $db->errorInfo()) : 'n/a');
	}

	print "done!";
?>
--EXPECTF--
Warning: PDO::__construct(file:%spdomuri.tst): failed to open stream: Invalid argument in %s on line %d
[002] URI=uri:file:%spdomuri.tst, DSN=mysql%sdbname=%s, File=%spdomuri.tst (%d bytes, 'mysql%sdbname=%s'), invalid data source URI

Warning: PDO::__construct(file:%spdomuri.tst): failed to open stream: Invalid argument in %s on line %d
[003] URI=uri:file:%spdomuri.tst, DSN=mysql%sdbname=%s, File=%spdomuri.tst (%d bytes, 'mysql%sdbname=letshopeinvalid%s'), chr(0) test, invalid data source URI
done!
