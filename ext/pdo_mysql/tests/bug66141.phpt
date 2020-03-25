--TEST--
Bug #66141 (mysqlnd quote function is wrong with NO_BACKSLASH_ESCAPES after failed query)
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();

$input = 'Something\', 1 as one, 2 as two  FROM dual; -- f';

$quotedInput0 = $db->quote($input);

$db->query('set session sql_mode="NO_BACKSLASH_ESCAPES"');

// injection text from some user input

$quotedInput1 = $db->quote($input);

$db->query('something that throws an exception');

$quotedInput2 = $db->quote($input);

var_dump($quotedInput0);
var_dump($quotedInput1);
var_dump($quotedInput2);
?>
done
--EXPECTF--
Warning: PDO::query(): SQLSTATE[42000]: Syntax error or access violation: 1064 You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'something that throws an exception' at line %d in %s on line %d
string(50) "'Something\', 1 as one, 2 as two  FROM dual; -- f'"
string(50) "'Something'', 1 as one, 2 as two  FROM dual; -- f'"
string(50) "'Something'', 1 as one, 2 as two  FROM dual; -- f'"
done
