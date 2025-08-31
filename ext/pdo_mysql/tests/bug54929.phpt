--TEST--
Bug #54929 (Parse error with single quote in sql comment (pdo-mysql))
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$pdodb = MySQLPDOTest::factory();

function testQuery($query) {
    global $pdodb;
    $stmt = $pdodb->prepare($query);

    if (!$stmt->execute(["foo"])) {
        var_dump($stmt->errorInfo());
    } else{
        var_dump($stmt->fetch(PDO::FETCH_ASSOC));
    }
}

testQuery("/* ' */ SELECT ? AS f1 /* ' */");
testQuery("/* '-- */ SELECT ? AS f1 /* *' */");
testQuery("/* ' */ SELECT ? AS f1 --';");
testQuery("/* ' */ SELECT ? AS f1 -- 'a;");
testQuery("/*'**/ SELECT ? AS f1 /* ' */");
testQuery("/*'***/ SELECT ? AS f1 /* ' */");
testQuery("/*'**a ***b / ****
******
**/ SELECT ? AS f1 /* ' */");

?>
--EXPECTF--
array(1) {
  ["f1"]=>
  string(3) "foo"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}

Warning: PDOStatement::execute(): SQLSTATE[42000]: Syntax error or access violation: 1064 You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near '--'' at line 1 in %s on line %d
array(3) {
  [0]=>
  string(5) "42000"
  [1]=>
  int(1064)
  [2]=>
  string(%d) "You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near '--'' at line 1"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}
array(1) {
  ["f1"]=>
  string(3) "foo"
}
