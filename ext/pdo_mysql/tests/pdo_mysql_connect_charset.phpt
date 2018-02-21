--TEST--
PDO_MYSQL: Defining a connection charset in the DSN
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

	/* Connect to mysql to determine the current charset so we can diffinate it */
	$link 		= MySQLPDOTest::factory();
	$charset 	= $link->query("SHOW VARIABLES LIKE 'character_set_connection'")->fetchObject()->value;

	/* Make sure that we don't attempt to set the current character set to make this case useful */
	$new_charset	= ($charset == 'latin1' ? 'ascii' : 'latin1');

	/* Done with the original connection, create a second link to test the character set being defined */
	unset($link);

	$link 		= MySQLPDOTest::factory('PDO', false, null, Array('charset' => $new_charset));
	$conn_charset 	= $link->query("SHOW VARIABLES LIKE 'character_set_connection'")->fetchObject()->value;

	if ($charset !== $conn_charset) {
		echo "done!\n";
	} else {
		echo "failed!\n";
	}
?>
--EXPECT--
done!
