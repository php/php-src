--TEST--
PDO Common: Bug #64705 errorInfo property of PDOException is null when PDO::__construct() fails
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if(getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

if(substr(getenv('PDOTEST_DSN'), 0, strlen('sqlite')) === 'sqlite') {
  putenv('PDOTEST_DSN=sqlite:.');
} else if(substr(getenv('PDOTEST_DSN'), 0, strlen('odbc')) === 'odbc') {
  putenv('PDOTEST_DSN=odbc:DRIVER=ibm-db2;HOSTNAME=localhost;PORT=50000;DATABASE=bad_database;PROTOCOL=TCPIP;UID=bad_user;PWD=bad_password;');
} else if(substr(getenv('PDOTEST_DSN'), 0, strlen('pgsql')) === 'pgsql') {
  putenv('PDOTEST_DSN=pgsql:host=localhost;port=54321;dbname=bad_database_gDm9VNHRkIxQ');
}

putenv('PDOTEST_USER=bad_username_gDm9VNHRkIxQ');
putenv('PDOTEST_PASS=bad_password_gDm9VNHRkIxQ');

try {
    $db = PDOTest::factory();
} catch (\PDOException $e) {
    var_dump($e->errorInfo);
    var_dump($e->getCode());
    var_dump($e->getMessage());
}
?>
===DONE===
--EXPECTF--
array(3) {
  [0]=>
  string(5) "%s"
  [1]=>
  int(%d)
  [2]=>
  string(%d) "SQLSTATE[%s]%A"
}
int(%d)
string(%d) "SQLSTATE[%s]%A"
===DONE===