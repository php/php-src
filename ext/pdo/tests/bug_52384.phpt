--TEST--
PDO Common: Bug #52384 (debugDumpParams does not emit the bind parameter value)
--SKIPIF--
<?php # vim:ft=php
# PDOTEST_DSN=sqlite::memory: REDIR_TEST_DIR=ext/pdo/tests/ TEST_PHP_EXECUTABLE=sapi/cli/php sapi/cli/php run-tests.php ext/pdo/tests/pdo_038.phpt 
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

$calories = 150;
$colour = 'red';
$tf = true;
$n = null;
$lob = str_repeat('a',1000);
$dob = '1978-10-17';

$errmode = $db->getAttribute(PDO::ATTR_ERRMODE);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$db->exec("DROP table bug_52384");
$db->setAttribute(PDO::ATTR_ERRMODE, $errmode);
$db->exec("CREATE table bug_52384 (calories int, colour varchar(64), tf int, n int, lob varchar(255), dob varchar(255))");

$sth = $db->prepare('SELECT 1 FROM bug_52384 WHERE calories=:calories AND colour=:colour AND tf=:tf AND n=:n AND lob=:lob AND dob=:dob');
$sth->bindParam(':calories', $calories, PDO::PARAM_INT);
$sth->bindValue(':colour', $colour, PDO::PARAM_STR);
$sth->bindValue(':tf', $tf, PDO::PARAM_BOOL);
$sth->bindValue(':n', $n, PDO::PARAM_NULL);
$sth->bindValue(':lob', $lob, PDO::PARAM_LOB);
$sth->bindValue(':dob', $dob, PDO::PARAM_STR|PDO::PARAM_INPUT_OUTPUT);

$sth->debugDumpParams();
unset($dbh);
?>
--EXPECT--
SQL: [113] SELECT 1 FROM bug_52384 WHERE calories=:calories AND colour=:colour AND tf=:tf AND n=:n AND lob=:lob AND dob=:dob
Params:  6
Key: Name: [9] :calories
paramno=-1
name=[9] ":calories"
is_param=1
param_type=1
is_input_output=0
param_value=150
Key: Name: [7] :colour
paramno=-1
name=[7] ":colour"
is_param=1
param_type=2
is_input_output=0
param_value=red
Key: Name: [3] :tf
paramno=-1
name=[3] ":tf"
is_param=1
param_type=5
is_input_output=0
param_value=true
Key: Name: [2] :n
paramno=-1
name=[2] ":n"
is_param=1
param_type=0
is_input_output=0
param_value=null
Key: Name: [4] :lob
paramno=-1
name=[4] ":lob"
is_param=1
param_type=3
is_input_output=0
param_value=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
Key: Name: [4] :dob
paramno=-1
name=[4] ":dob"
is_param=1
param_type=2
is_input_output=1
param_value=1978-10-17

