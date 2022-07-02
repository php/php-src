--TEST--
Bug #46292 (PDO::setFetchMode() shouldn't requires the 2nd arg when using FETCH_CLASSTYPE)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php

    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $pdoDb = MySQLPDOTest::factory();


    class myclass {
        public $value;

        public function __construct() {
            printf("%s()\n", __METHOD__);
        }
    }

    class myclass2 extends myclass { }

    $pdoDb->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    $pdoDb->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

    $pdoDb->query('DROP TABLE IF EXISTS testz');

    $pdoDb->query('CREATE TABLE testz (name VARCHAR(20) NOT NULL, value INT)');

    $pdoDb->query("INSERT INTO testz VALUES ('myclass', 1), ('myclass2', 2), ('myclass', NULL), ('myclass3', NULL)");

    $stmt = $pdoDb->prepare("SELECT * FROM testz");

    var_dump($stmt->setFetchMode(PDO::FETCH_CLASS | PDO::FETCH_CLASSTYPE | PDO::FETCH_GROUP));
    $stmt->execute();

    var_dump($stmt->fetch());
    var_dump($stmt->fetch());
    var_dump($stmt->fetchAll());
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS testz');
?>
--EXPECTF--
bool(true)
myclass::__construct()
object(myclass)#%d (1) {
  ["value"]=>
  string(1) "1"
}
myclass::__construct()
object(myclass2)#%d (1) {
  ["value"]=>
  string(1) "2"
}
myclass::__construct()
array(2) {
  [0]=>
  object(myclass)#%d (1) {
    ["value"]=>
    NULL
  }
  [1]=>
  object(stdClass)#%d (1) {
    ["value"]=>
    NULL
  }
}
