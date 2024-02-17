--TEST--
GH-13384 Fixed GH-13167 Fixed the behavior when an inappropriate value was passed to `bindValue` and `bindParam`.
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
$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

$stringableObject = new class () implements Stringable {
    public function __toString(): string
    {
        return '555';
    }
};

echo "Stringable object, bindValue:\n";
$stmt = $db->prepare('SELECT (?)');
$stmt->bindValue(1, $stringableObject, PDO::PARAM_INT);
$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
echo "\n";

echo "Normal object, bindValue:\n";
try {
    $stmt = $db->prepare('SELECT (?)');
    $stmt->bindValue(1, new stdClass(), PDO::PARAM_INT);
    $stmt->execute();
} catch (Throwable $e) {
    echo $e->getMessage()."\n\n";
}

echo "Array, bindParam:\n";
try {
    $stmt = $db->prepare('SELECT (?)');
    $param = ['aaa'];
    $stmt->bindParam(1, $param, PDO::PARAM_INT);
    $stmt->execute();
} catch (Throwable $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Stringable object, bindValue:
array(1) {
  [0]=>
  array(1) {
    ["?"]=>
    string(3) "555"
  }
}

Normal object, bindValue:
SQLSTATE[HY105]: Invalid parameter type: Expected a scalar value or null

Array, bindParam:
SQLSTATE[HY105]: Invalid parameter type: Expected a scalar value or null
