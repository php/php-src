--TEST--
Bug #44707 (The MySQL PDO driver resets variable content after bindParam on tinyint field)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipVersionThanLess(40100);
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

function bug_44707($db) {
    $db->exec('CREATE TABLE test_44707(id INT, mybool TINYINT)');

    $id = 1;
    $mybool = false;
    var_dump($mybool);

    $stmt = $db->prepare('INSERT INTO test_44707(id, mybool) VALUES (?, ?)');
    $stmt->bindParam(1, $id);
    $stmt->bindParam(2, $mybool, PDO::PARAM_BOOL);
    var_dump($mybool);

    $stmt->execute();
    var_dump($mybool);

    $stmt = $db->query('SELECT * FROM test_44707');
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    $stmt = $db->prepare('INSERT INTO test_44707(id, mybool) VALUES (?, ?)');
    $stmt->bindParam(1, $id);
    // INT and integer work well together
    $stmt->bindParam(2, $mybool, PDO::PARAM_INT);
    $stmt->execute();

    $stmt = $db->query('SELECT * FROM test_44707');
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
}


/*
// This is beyond the control of the driver... - the driver never gets in touch with bound values
print "Emulated Prepared Statements\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
bug_44707($db);
*/

print "Native Prepared Statements\n";
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
bug_44707($db);

print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_44707');
?>
--EXPECT--
Native Prepared Statements
bool(false)
bool(false)
bool(false)
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["mybool"]=>
    string(1) "0"
  }
}
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["mybool"]=>
    string(1) "0"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["mybool"]=>
    string(1) "0"
  }
}
done!
