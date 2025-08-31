--TEST--
Bug #79132: PDO re-uses parameter values from earlier calls to execute()
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

$pdo = MySQLPDOTest::factory();
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
test($pdo);
echo "\n";
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
test($pdo);

function test($pdo) {
    $stmt = $pdo->prepare('SELECT ? a, ? b');

    $set = [
        ['a', 'b'],
        ['x'],      /* second parameter is missing */
        [1 => 'y'], /* first parameter is missing */
    ];

    foreach ($set as $params) {
        try {
            var_dump($stmt->execute($params), $stmt->fetchAll(PDO::FETCH_ASSOC));
        } catch (PDOException $error) {
            echo $error->getMessage() . "\n";
        }
    }
}

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  array(2) {
    ["a"]=>
    string(1) "a"
    ["b"]=>
    string(1) "b"
  }
}
SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens
SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens

bool(true)
array(1) {
  [0]=>
  array(2) {
    ["a"]=>
    string(1) "a"
    ["b"]=>
    string(1) "b"
  }
}
SQLSTATE[HY093]: Invalid parameter number
SQLSTATE[HY093]: Invalid parameter number
