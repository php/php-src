--TEST--
GH-20214 (PDO::FETCH_DEFAULT unexpected behavior with PDOStatement::setFetchMode)
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

$db->exec('CREATE TABLE gh20214 (c1 VARCHAR(10), c2 VARCHAR(10))');
$db->exec("INSERT INTO gh20214 VALUES ('v1', 'v2')");

$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);

// setFetchMode with FETCH_DEFAULT should use connection default (FETCH_OBJ)
$stmt = $db->query('SELECT c1, c2 FROM gh20214');
$stmt->setFetchMode(PDO::FETCH_DEFAULT);
$row = $stmt->fetch();
var_dump($row instanceof stdClass);
var_dump($row->c1);

// fetch with FETCH_DEFAULT should also use connection default
$stmt = $db->query('SELECT c1, c2 FROM gh20214');
$row = $stmt->fetch(PDO::FETCH_DEFAULT);
var_dump($row instanceof stdClass);

// fetchAll with FETCH_DEFAULT should also use connection default
$stmt = $db->query('SELECT c1, c2 FROM gh20214');
$rows = $stmt->fetchAll(PDO::FETCH_DEFAULT);
var_dump($rows[0] instanceof stdClass);

// setFetchMode then fetch without argument
$stmt = $db->query('SELECT c1, c2 FROM gh20214');
$stmt->setFetchMode(PDO::FETCH_DEFAULT);
$row = $stmt->fetch();
var_dump($row instanceof stdClass);

// query() with FETCH_DEFAULT as second argument
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_NUM);
$stmt = $db->query('SELECT c1, c2 FROM gh20214', PDO::FETCH_DEFAULT);
$row = $stmt->fetch();
var_dump(is_array($row) && isset($row[0]));
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "gh20214");
?>
--EXPECT--
bool(true)
string(2) "v1"
bool(true)
bool(true)
bool(true)
bool(true)
