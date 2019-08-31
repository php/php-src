--TEST--
MySQL PDO->__construct(), libmysql only options
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
if (MySQLPDOTest::isPDOMySQLnd())
	die("skip libmysql only options")
?>
--FILE--
<?php
	require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	function set_option_and_check($offset, $option, $value, $option_desc, $ignore_diff = false) {

		$dsn = MySQLPDOTest::getDSN();
		$user = PDO_MYSQL_TEST_USER;
		$pass = PDO_MYSQL_TEST_PASS;

		try {
			$db = new PDO($dsn, $user, $pass, array($option => $value));
			if (!is_object($db) || (!$ignore_diff && ($value !== ($tmp = @$db->getAttribute($option)))))
				printf("[%03d] Execting '%s'/%s got '%s'/%s' for options '%s'\n",
					$offset,
					$value, gettype($value),
					$tmp, gettype($tmp),
					$option_desc);
		} catch (PDOException $e) {
			printf("[%03d] %s\n", $offset, $e->getMessage());
		}

	}

	try {

		$dsn = MySQLPDOTest::getDSN();
		$user = PDO_MYSQL_TEST_USER;
		$pass = PDO_MYSQL_TEST_PASS;

		$valid_options = array();
		$valid_options[PDO::MYSQL_ATTR_MAX_BUFFER_SIZE] = 'PDO::MYSQL_ATTR_MAX_BUFFER_SIZE';
		$valid_options[PDO::MYSQL_ATTR_INIT_COMMAND] = 'PDO::MYSQL_ATTR_INIT_COMMAND';
		$valid_options[PDO::MYSQL_ATTR_READ_DEFAULT_FILE] = 'PDO::MYSQL_ATTR_READ_DEFAULT_FILE';
		$valid_options[PDO::MYSQL_ATTR_READ_DEFAULT_GROUP] = 'PDO::MYSQL_ATTR_READ_DEFAULT_GROUP';

		$defaults[PDO::MYSQL_ATTR_MAX_BUFFER_SIZE] = 1048576;
		/* TODO getAttribute() does not handle it */
		$defaults[PDO::MYSQL_ATTR_INIT_COMMAND] = '';
		$defaults[PDO::MYSQL_ATTR_READ_DEFAULT_FILE] = false;
		$defaults[PDO::MYSQL_ATTR_READ_DEFAULT_GROUP] = false;

		$db = new PDO($dsn, $user, $pass);
		foreach ($valid_options as $option => $name) {
			/* TODO getAttribute() is pretty poor in supporting the options, suppress errors */
			$tmp = @$db->getAttribute($option);
			if ($tmp !== $defaults[$option])
				printf("[001] Expecting default value for '%s' of '%s'/%s, getAttribute() reports setting '%s'/%s\n",
					$name, $defaults[$option], gettype($defaults[$option]),
					$tmp, gettype($tmp));
		}

		set_option_and_check(26, PDO::MYSQL_ATTR_READ_DEFAULT_FILE, true, 'PDO::MYSQL_ATTR_READ_DEFAULT_FILE');
		set_option_and_check(27, PDO::MYSQL_ATTR_READ_DEFAULT_FILE, false, 'PDO::MYSQL_ATTR_READ_DEFAULT_FILE');

		set_option_and_check(30, PDO::MYSQL_ATTR_MAX_BUFFER_SIZE, -1, 'PDO::MYSQL_ATTR_MAX_BUFFER_SIZE', true);
		set_option_and_check(31, PDO::MYSQL_ATTR_MAX_BUFFER_SIZE, PHP_INT_MAX, 'PDO::MYSQL_ATTR_MAX_BUFFER_SIZE');
		set_option_and_check(32, PDO::MYSQL_ATTR_MAX_BUFFER_SIZE, 1, 'PDO::MYSQL_ATTR_MAX_BUFFER_SIZE');


	} catch (PDOException $e) {
		printf("[001] %s, [%s] %s\n",
			$e->getMessage(),
			(is_object($db)) ? $db->errorCode() : 'n/a',
			(is_object($db)) ? implode(' ', $db->errorInfo()) : 'n/a');
	}

	print "done!";
?>
--EXPECT--
[001] Expecting default value for 'PDO::MYSQL_ATTR_INIT_COMMAND' of ''/string, getAttribute() reports setting ''/boolean
[026] Execting '1'/boolean got ''/boolean' for options 'PDO::MYSQL_ATTR_READ_DEFAULT_FILE'
done!
