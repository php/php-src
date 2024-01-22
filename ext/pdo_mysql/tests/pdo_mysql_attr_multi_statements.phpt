--TEST--
PDO::MYSQL_ATTR_MULTI_STATEMENTS
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--INI--
error_reporting=E_ALL
--FILE--
<?php
    require_once __DIR__ . '/inc/mysql_pdo_test.inc';

    $dsn = MySQLPDOTest::getDSN();
    $user = PDO_MYSQL_TEST_USER;
    $pass = PDO_MYSQL_TEST_PASS;

    $table = 'pdo_mysql_attr_multi_statements';

    $db = new PDO($dsn, $user, $pass);
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

    $create = sprintf('CREATE TABLE %s(id INT)', $table);
    $db->exec($create);
    $db->exec(sprintf('INSERT INTO %s(id) VALUES (1)', $table));
    $stmt = $db->query(sprintf('SELECT * FROM %s; INSERT INTO %s(id) VALUES (2)', $table, $table));
    $stmt->closeCursor();
    $info = $db->errorInfo();
    var_dump($info[0]);
    $stmt = $db->query(sprintf('SELECT id FROM %s', $table));
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        // A single query with a trailing delimiter.
    $stmt = $db->query('SELECT 1 AS value;');
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    // New connection, does not allow multiple statements.
    $db = new PDO($dsn, $user, $pass, array(PDO::MYSQL_ATTR_MULTI_STATEMENTS => false));
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
    $stmt = $db->query(sprintf('SELECT * FROM %s; INSERT INTO %s(id) VALUES (3)', $table, $table));
    var_dump($stmt);
    $info = $db->errorInfo();
    var_dump($info[0]);

    $stmt = $db->query(sprintf('SELECT id FROM %s', $table));
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        // A single query with a trailing delimiter.
        $stmt = $db->query('SELECT 1 AS value;');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS pdo_mysql_attr_multi_statements');
?>
--EXPECTF--
string(5) "00000"
array(2) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    ["id"]=>
    string(1) "2"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["value"]=>
    string(1) "1"
  }
}

Warning: PDO::query(): SQLSTATE[42000]: Syntax error or access violation: 1064 You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'INSERT INTO %s(id) VALUES (3)' at line 1 in %s on line %d
bool(false)
string(5) "42000"
array(2) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    ["id"]=>
    string(1) "2"
  }
}
array(1) {
  [0]=>
  array(1) {
    ["value"]=>
    string(1) "1"
  }
}
done!
