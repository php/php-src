--TEST--
MySQL PDO->__construct() - Generic + DSN
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	function tryandcatch($offset, $code) {

		try {
			eval($code);
			assert(sprintf("[%03d] Should have failed\n", $offset) != '');
		} catch (PDOException $e) {
			return sprintf("[%03d] %s, [%s] %s\n",
				$offset,
				$e->getMessage(),
				(isset($db) && is_object($db)) ? $db->errorCode() : 'n/a',
				(isset($db) && is_object($db)) ? implode(' ', $db->errorInfo()) : 'n/a');
		}

		return '';
	}

	try {

		if (NULL !== ($db = @new PDO()))
			printf("[001] Too few parameters\n");

		print tryandcatch(2, '$db = new PDO(chr(0));');
		print tryandcatch(3, '$db = new PDO("a" . chr(0) . "b");');
		print tryandcatch(4, '$db = new PDO("MYSQL");');
		print tryandcatch(5, '$db = new PDO("mysql");');
		print tryandcatch(6, '$db = new PDO("mysql ");');
		print tryandcatch(7, '$db = new PDO("fantasyandfriends :");');

		$dsn = PDO_MYSQL_TEST_DSN;
		// MySQL Server might accept anonymous connections, don't
		// print anything
		tryandcatch(8, '$db = new PDO("' . $dsn . '");');

		$user = 'dontcreatesuchauser';
		$pass = 'withthispassword';
		print tryandcatch(9, '$db = new PDO("' . $dsn . '", "' . $user . '", "' . $pass . '");');

		// should fail
		$dsn = 'mysql:';
		// don't print the message since it can be different
		tryandcatch(10, '$db = new PDO("' . $dsn . '", "' . $user . '", "' . $pass . '");');

		$dsn = PDO_MYSQL_TEST_DSN;
		$user = PDO_MYSQL_TEST_USER;
		$pass	= PDO_MYSQL_TEST_PASS;
		// should work...
		$db = new PDO($dsn, $user, $pass);

		// Reaction on host not specified differs for different configs, so no printing
		$dsn = 'mysql:invalid=foo';
		tryandcatch(11, '$db = new PDO("' . $dsn . '", "' . $user . '", "' . $pass . '");');

		$dsn = 'mysql:' . str_repeat('howmuch=canpdoeat;', 1000);
		tryandcatch(12, '$db = new PDO("' . $dsn . '", "' . $user . '", "' . $pass . '");');

		$dsn = 'mysql:' . str_repeat('abcdefghij', 1024 * 10) . '=somevalue';
		tryandcatch(13, '$db = new PDO("' . $dsn . '", "' . $user . '", "' . $pass . '");');

		if (PDO_MYSQL_TEST_HOST) {
			$host = PDO_MYSQL_TEST_HOST;
			$invalid_host = $host . 'invalid';

			// last host specification should be the one used
			$dsn = MySQLPDOTest::getDSN(array('host' => $host), 'host=' . $invalid_host);
			try { $db = @new PDO($dsn, $user, $pass); assert(false); printf("%s\n", $dsn); } catch (PDOException $e) {
				$tmp = $e->getMessage();
				if (!stristr($tmp, 'HY000') && !stristr($tmp, '2005') && !stristr($tmp, '2002'))
					printf("[014] Cannot find proper error codes: %s\n", $tmp);
			}

			$dsn = MySQLPDOTest::getDSN(array('host' => $invalid_host), 'host=' . $host);
			try { $db = @new PDO($dsn, $user, $pass); } catch (PDOException $e) {
				printf("[015] DSN=%s, %s\n", $dsn, $e->getMessage());
			}

			$invalid_host = '-' . chr(0);

			$dsn = MySQLPDOTest::getDSN(array('host' => $invalid_host));
			try { $db = @new PDO($dsn, $user, $pass); assert(false); printf("%s\n", $dsn); } catch (PDOException $e) {
				$tmp = $e->getMessage();
				if (!stristr($tmp, 'HY000') && !stristr($tmp, '2005') && !stristr($tmp, '2002'))
					printf("[016] Cannot find proper error codes: %s\n", $tmp);
			}

			// parsing should not get confused by chr(0)
			$dsn = MySQLPDOTest::getDSN(array('host' => $invalid_host), 'host=' . $host);
			try { $db = @new PDO($dsn, $user, $pass); } catch (PDOException $e) {
				printf("[017] DSN=%s, %s\n", $dsn, $e->getMessage());
			}

		}

		// what about long values for a valid option ...
		// hostnames > 1024 chars break on some NIS-enabled FreeBSD...
		$dsn = MySQLPDOTest::getDSN(array('host' => str_repeat('0123456789', 100)));
		try { $db = @new PDO($dsn, $user, $pass); assert(false); printf("%s\n", $dsn); } catch (PDOException $e) {
			$tmp = $e->getMessage();
			if (!stristr($tmp, 'HY000') && !stristr($tmp, '2005') && !stristr($tmp, '2002'))
				printf("[018] Cannot find proper error codes: %s\n", $tmp);
		}

		if (PDO_MYSQL_TEST_PORT && (PDO_MYSQL_TEST_SOCKET == '')) {
			// Playing with the port makes only sense if no socket gets used

			$port = PDO_MYSQL_TEST_PORT;
			// let's hope we don't hit a MySQL running on that port...
			$invalid_port = $port * 2;

			$dsn = MySQLPDOTest::getDSN(array('port' => $port), 'port=' . $invalid_port);
			try { $db = @new PDO($dsn, $user, $pass); assert(false); printf("%s\n", $dsn); } catch (PDOException $e) {
				$tmp = $e->getMessage();
				if (!stristr($tmp, 'HY000') && !stristr($tmp, '2005'))
					printf("[019] Cannot find proper error codes: %s\n", $tmp);
			}

			$dsn = MySQLPDOTest::getDSN(array('port' => $invalid_port), 'port=' . $port);
			try { $db = @new PDO($dsn, $user, $pass); } catch (PDOException $e) {
				printf("[020] DSN=%s, %s\n", $dsn, $e->getMessage());
			}

			$invalid_port = 'abc';
			$dsn = MySQLPDOTest::getDSN(array('port' => $port), 'port=' . $invalid_port);
			try {
				$db = @new PDO($dsn, $user, $pass);
				// atoi('abc') = 0, 0 -> fallback to default 3306 -> may or may not fail!
			} catch (PDOException $e) {
			}

		}

		if (PDO_MYSQL_TEST_DB) {
			$db = PDO_MYSQL_TEST_DB;
			$invalid_db = 'letshopeitdoesnotexist';

			$dsn = MySQLPDOTest::getDSN(array('dbname' => $db), 'dbname=' . $invalid_db);
			try { $db = @new PDO($dsn, $user, $pass); assert(false); printf("%s\n", $dsn); } catch (PDOException $e) {
				$tmp = $e->getMessage();
				if (!stristr($tmp, '42000') && !stristr($tmp, '1049'))
					printf("[022] Cannot find proper error codes: %s\n", $tmp);
			}

			$dsn = MySQLPDOTest::getDSN(array('dbname' => $invalid_db), 'dbname=' . $db);
			try { $db = @new PDO($dsn, $user, $pass); } catch (PDOException $e) {
				printf("[023] DSN=%s, %s\n", $dsn, $e->getMessage());
			}

		}

		if (PDO_MYSQL_TEST_SOCKET && (stristr(PDO_MYSQL_TEST_DSN, PDO_MYSQL_TEST_SOCKET) !== false)) {
			$socket = PDO_MYSQL_TEST_SOCKET;
			$invalid_socket = '/lets/hope/it/does/not/exist';

			$dsn = MySQLPDOTest::getDSN(array('unix_socket' => $socket), 'unix_socket=' . $invalid_socket);
			try { $db = @new PDO($dsn, $user, $pass); assert(false); printf("%s\n", $dsn); } catch (PDOException $e) {
				$tmp = $e->getMessage();
				if (!stristr($tmp, 'HY000') && !stristr($tmp, '2002'))
					printf("[024] Cannot find proper error codes: %s\n", $tmp);
			}

			$dsn = MySQLPDOTest::getDSN(array('unix_socket' => $invalid_socket), 'unix_socket=' . $socket);
			try { $db = @new PDO($dsn, $user, $pass); } catch (PDOException $e) {
				printf("[025] DSN=%s, %s\n", $dsn, $e->getMessage());
			}

		}

		$have_charset_support = false;
		$dsn = MySQLPDOTest::getDSN();
		try {
			$db = new PDO($dsn, $user, $pass);
			$stmt = $db->query('SELECT VERSION() as _version');
			$version = $stmt->fetch(PDO::FETCH_ASSOC);

			$tmp = explode('.', $version['_version']);
			if ((count($tmp) == 3) &&
					(($tmp[0] >= 4 && $tmp[1] >= 1) || ($tmp[0] >= 5))) {
				// MySQL Server 4.1 - charset support available
				$have_charset_support = true;
			}

		} catch (PDOException $e) {
			printf("[026] DSN=%s, %s\n", $dsn, $e->getMessage());
		}

		if (PDO_MYSQL_TEST_CHARSET) {
			$charset = PDO_MYSQL_TEST_CHARSET;
			$invalid_charset = 'invalid';

			if ($have_charset_support) {
				$dsn = MySQLPDOTest::getDSN();
				$db = new PDO($dsn, $user, $pass);
				$stmt = $db->query(sprintf('SHOW CHARACTER SET LIKE "%s"', $charset));
				$tmp = $stmt->fetch(PDO::FETCH_ASSOC);
				$have_charset = (empty($tmp)) ? false : true;

				if ($have_charset) {
					$dsn = MySQLPDOTest::getDSN(array('charset' => $charset), 'charset=' . $invalid_charset);
					try {
						$db = @new PDO($dsn, $user, $pass);
						/* NOTE: MySQL does a fallback to the charset suggested during the handshake - no error - no bug! */
					} catch (PDOException $e) {
						$tmp = $e->getMessage();
						/* TODO: add proper codes */
						if (!stristr($tmp, 'sqlstatecode') || !stristr($tmp, 'mysqlinternalerrcode'))
							printf("[027] TODO - Cannot find proper error codes: %s\n", $tmp);
					}

					$dsn = MySQLPDOTest::getDSN(array('charset' => $invalid_charset), 'charset=' . $charset);
					try {
						$db = @new PDO($dsn, $user, $pass);
						/* Strictly speaking we should test more: character_set_client, character_set_results, and character_set_connection */
						$stmt = $db->query('SELECT @@character_set_connection AS _charset');
						$tmp = $stmt->fetch(PDO::FETCH_ASSOC);
						if ($tmp['_charset'] != $charset)
							printf("[028] Character sets has not been set, @@character_set_connection reports '%s', expecting '%s'",
								$tmp['_charset'], $charset);
					} catch (PDOException $e) {
						printf("[029] DSN=%s, %s\n", $dsn, $e->getMessage());
					}
				} else {
					printf("[030] You're trying to run the tests with charset '%s' which seems not supported by the server!", $charset);
				}

			}

		}

		if ($have_charset_support) {
			// In case the PDO_MYSQL_TEST_CHARSET interferes with any defaults
			// we do another test to verify that the charset has been set.
			$dsn = MySQLPDOTest::getDSN();
			$db = new PDO($dsn, $user, $pass);
			$stmt = $db->query('SHOW CHARACTER SET LIKE "latin1"');
			$tmp = $stmt->fetch(PDO::FETCH_ASSOC);
			$have_latin1 =(empty($tmp)) ? false : true;
			$stmt = $db->query('SHOW CHARACTER SET LIKE "latin2"');
			$tmp = $stmt->fetch(PDO::FETCH_ASSOC);
			$have_latin2 =(empty($tmp)) ? false : true;

			if ($have_latin1 && $have_latin2) {
				// very likely we do have both of them...
				try {
					$dsn = MySQLPDOTest::getDSN(array('charset' => 'latin1'));
					$db = new PDO($dsn, $user, $pass);
					$stmt = $db->query('SELECT @@character_set_connection AS _charset');
					$tmp = $stmt->fetch(PDO::FETCH_ASSOC);
					if ($tmp['_charset'] != 'latin1')
						printf("[031] DSN = %s, Character sets has not been set, @@character_set_connection reports '%s', expecting '%s'",
							$dsn, $tmp['_charset'], 'latin1');

				} catch (PDOException $e) {
					printf("[032] %s\n", $e->getMessage());
				}

				try {
					$dsn = MySQLPDOTest::getDSN(array('charset' => 'latin2'));
					$db = new PDO($dsn, $user, $pass);
					$stmt = $db->query('SELECT @@character_set_connection AS _charset');
					$tmp = $stmt->fetch(PDO::FETCH_ASSOC);
					if ($tmp['_charset'] != 'latin2')
						printf("[033] DSN = %s, character sets has not been set, @@character_set_connection reports '%s', expecting '%s'",
							$dsn, $tmp['_charset'], 'latin2');

				} catch (PDOException $e) {
					printf("[034] %s\n", $e->getMessage());
				}

			}
		}

	} catch (PDOException $e) {
		printf("[001] %s, [%s] %s\n",
			$e->getMessage(),
			(is_object($db)) ? $db->errorCode() : 'n/a',
			(is_object($db)) ? implode(' ', $db->errorInfo()) : 'n/a');
	}

	print "done!";
?>
--EXPECTF--
[002] invalid data source name, [n/a] n/a
[003] invalid data source name, [n/a] n/a
[004] invalid data source name, [n/a] n/a
[005] invalid data source name, [n/a] n/a
[006] invalid data source name, [n/a] n/a
[007] could not find driver, [n/a] n/a
[009] SQLSTATE[%s] [1045] Access denied for user 'dont%s'@'%s' (using password: YES), [n/a] n/a
[017] DSN=%s, SQLSTATE[%s] [%d] %s
done!
