--TEST--
PDO Common: PDOStatement::activeQueryString()
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

$db = PDOTest::factory();
if (!$db->getAttribute(PDO::ATTR_EMULATE_PREPARES) && !$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true)) die('skip driver cannot emulate prepared statements');
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$db = PDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

$quoted_true = $db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'pgsql' ? "'t'" : '1';

$stmt = $db->query('SELECT 1');
var_dump($stmt->activeQueryString()); // works with statements without bound values

$stmt = $db->prepare('SELECT :bool, :int, :string, :null');
$stmt->bindValue(':bool', true, PDO::PARAM_BOOL);
$stmt->bindValue(':int', 123, PDO::PARAM_INT);
$stmt->bindValue(':string', 'foo', PDO::PARAM_STR);
$stmt->bindValue(':null', null, PDO::PARAM_NULL);

var_dump($stmt->activeQueryString()); // will return unparsed query before execution

$stmt->execute();

// will return parsed query after execution
var_dump($stmt->activeQueryString() == "SELECT " . $quoted_true . ", 123, 'foo', NULL");
// can be called repeatedly
var_dump($stmt->activeQueryString() == "SELECT " . $quoted_true . ", 123, 'foo', NULL");

$stmt->execute();

// works if the statement is executed again
var_dump($stmt->activeQueryString() == "SELECT " . $quoted_true . ", 123, 'foo', NULL");

$stmt->bindValue(':int', 456, PDO::PARAM_INT);
$stmt->execute();

// works with altered values
var_dump($stmt->activeQueryString() == "SELECT " . $quoted_true . ", 456, 'foo', NULL");

?>
--EXPECT--
string(8) "SELECT 1"
string(34) "SELECT :bool, :int, :string, :null"
bool(true)
bool(true)
bool(true)
bool(true)
