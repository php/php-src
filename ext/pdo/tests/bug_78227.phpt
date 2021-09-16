--TEST--
Bug #78227 Prepared statements ignore RENAME COLUMN
--EXTENSIONS--
pdo
--SKIPIF--
<?php
	$dir = getenv('REDIR_TEST_DIR');
	if (false == $dir) die('skip no driver');
	require_once $dir . 'pdo_test.inc';
	PDOTest::skip();
?>
--FILE--
<?php
	if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
	require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
	$db = PDOTest::factory();
	$db->exec("drop table bug_78227");

	$db->exec("CREATE TABLE bug_78227 (id INTEGER PRIMARY KEY NOT NULL, foo VARCHAR(255) NOT NULL)");
	$db->exec("INSERT INTO bug_78227 (id, foo) VALUES (10, 'test')");
	$stmt = $db->prepare("SELECT * FROM bug_78227 WHERE id = :id");
	$stmt->execute(['id' => 10]);
	print_r($stmt->fetch(PDO::FETCH_ASSOC));

	$driver = $db->getAttribute(PDO::ATTR_DRIVER_NAME);
	switch(true){
		case "mysql" == $driver:
			$db->exec("ALTER TABLE bug_78227 change foo bat varchar(20)");
			break;
		case "sqlite" == $driver:
		case "oci" == $driver:
		case "pgsql" == $driver:
			$db->exec("ALTER TABLE bug_78227 rename column foo to bar");
			break;
		case "firebird" == $driver:
			$db->exec("ALTER TABLE bug_78227 alter column foo to bar");
			break;
		case "mssql" == $driver:
			$db->exec("execute sp_rename 'bug_78227.foo','bar','column'");
			break;
		case "sybase" == $driver:
			$db->exec("sp_rename 'bug_78227.foo','bar','column'");
			break;
		default:
			die('skip unknown driver');
	}

	$res = $stmt->execute(['id' => 10]);
	print_r($stmt->fetch(PDO::FETCH_ASSOC));
?>
--CLEAN--
<?php
	if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
	require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
	$db = PDOTest::factory();
	$db->exec("drop table bug_78227");
?>
--EXPECT--
Array
(
    [id] => 10
    [foo] => test
)
Array
(
    [id] => 10
    [bar] => test
)