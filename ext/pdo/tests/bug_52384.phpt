--TEST--
PDO Common: Bug #52384 (debugDumpParams does not emit the bind parameter value)
--SKIPIF--
<?php # vim:ft=php
# PDOTEST_DSN=sqlite::memory: REDIR_TEST_DIR=ext/pdo/tests/ TEST_PHP_EXECUTABLE=sapi/cli/php sapi/cli/php run-tests.php ext/pdo/tests/bug_52384.phpt 
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$a = 150;
$b = 'red';
$c = true;
$d = null;
$e = str_repeat('a',1000);
$f = '1978-10-17';

$db->exec("CREATE TABLE test (a INT, b VARCHAR(10), c INT, d INT, e TEXT, f VARCHAR(10))");
$sth = $db->prepare(
    'SELECT * FROM test WHERE a=:a AND b=:b AND c=:c AND d=:d AND e=:e AND f=:f'
);
$sth->bindParam(':a', $a, PDO::PARAM_INT);
$sth->bindValue(':b', $b, PDO::PARAM_STR);
$sth->bindValue(':c', $c, PDO::PARAM_BOOL);
$sth->bindValue(':d', $d, PDO::PARAM_NULL);
$sth->bindValue(':e', $e, PDO::PARAM_LOB);
$sth->bindValue(':f', $f, PDO::PARAM_STR|PDO::PARAM_INPUT_OUTPUT);

$sth->debugDumpParams();
unset($dbh);
?>
--EXPECTF--
SQL: [74] SELECT * FROM test WHERE a=:a AND b=:b AND c=:c AND d=:d AND e=:e AND f=:f
Params:  6
Key: Name: [2] :a
paramno=%s
name=[2] ":a"
is_param=1
param_type=1
is_input_output=0
param_value=150
Key: Name: [2] :b
paramno=%s
name=[2] ":b"
is_param=1
param_type=2
is_input_output=0
param_value=red
Key: Name: [2] :c
paramno=%s
name=[2] ":c"
is_param=1
param_type=5
is_input_output=0
param_value=true
Key: Name: [2] :d
paramno=%s
name=[2] ":d"
is_param=1
param_type=0
is_input_output=0
param_value=null
Key: Name: [2] :e
paramno=%s
name=[2] ":e"
is_param=1
param_type=3
is_input_output=0
param_value=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
Key: Name: [2] :f
paramno=%s
name=[2] ":f"
is_param=1
param_type=2
is_input_output=1
param_value=1978-10-17

