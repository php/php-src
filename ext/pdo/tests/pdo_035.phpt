--TEST--
PDO Common: PDORow + get_parent_class()
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

const TABLE_NAME = 'test_pdo_35_pdo_row';
$db->exec('CREATE TABLE ' . TABLE_NAME .' (id int, name varchar(10))');
$db->exec('INSERT INTO ' . TABLE_NAME .' VALUES (23, \'\')');

$stmt = $db->prepare('SELECT id, name FROM ' . TABLE_NAME);
$stmt->execute();
$result = $stmt->fetch(PDO::FETCH_LAZY);

var_dump($result);
var_dump(get_parent_class($result));

foreach ([0, "0", "id", "name", 1] as $offset) {
    echo 'Offset: ', var_export($offset), PHP_EOL;
    $offsetRef = &$offset;

    echo 'Dimension:', PHP_EOL;
    echo 'Isset:', PHP_EOL;
    var_dump(isset($result[$offset]));
    var_dump(isset($result[$offsetRef]));
    echo 'Empty:', PHP_EOL;
    var_dump(empty($result[$offset]));
    var_dump(empty($result[$offsetRef]));
    echo 'Null coalesce:', PHP_EOL;
    var_dump($result[$offset] ?? "default");
    var_dump($result[$offsetRef] ?? "default");
    echo 'Read:', PHP_EOL;
    var_dump($result[$offset]);
    var_dump($result[$offsetRef]);
    echo 'Property:', PHP_EOL;
    echo 'Isset:', PHP_EOL;
    var_dump(isset($result->{$offset}));
    var_dump(isset($result->{$offsetRef}));
    echo 'Empty:', PHP_EOL;
    var_dump(empty($result->{$offset}));
    var_dump(empty($result->{$offsetRef}));
    echo 'Null coalesce:', PHP_EOL;
    var_dump($result->{$offset} ?? "default");
    var_dump($result->{$offsetRef} ?? "default");
    echo 'Read:', PHP_EOL;
    var_dump($result->{$offset});
    var_dump($result->{$offsetRef});
}

echo 'Errors:', PHP_EOL;
try {
    $result[0] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $result[] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $refResult = &$result[0];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $refResult = &$result[];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($result[0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $result->foo = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($result->foo);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
object(PDORow)#3 (3) {
  ["queryString"]=>
  string(40) "SELECT id, name FROM test_pdo_35_pdo_row"
  ["id"]=>
  string(2) "23"
  ["name"]=>
  string(0) ""
}
bool(false)
Offset: 0
Dimension:
Isset:
bool(true)
bool(true)
Empty:
bool(false)
bool(false)
Null coalesce:
string(2) "23"
string(2) "23"
Read:
string(2) "23"
string(2) "23"
Property:
Isset:
bool(true)
bool(true)
Empty:
bool(false)
bool(false)
Null coalesce:
string(2) "23"
string(2) "23"
Read:
string(2) "23"
string(2) "23"
Offset: '0'
Dimension:
Isset:
bool(true)
bool(true)
Empty:
bool(false)
bool(false)
Null coalesce:
string(2) "23"
string(2) "23"
Read:
string(2) "23"
string(2) "23"
Property:
Isset:
bool(true)
bool(true)
Empty:
bool(false)
bool(false)
Null coalesce:
string(2) "23"
string(2) "23"
Read:
string(2) "23"
string(2) "23"
Offset: 'id'
Dimension:
Isset:
bool(true)
bool(true)
Empty:
bool(false)
bool(false)
Null coalesce:
string(2) "23"
string(2) "23"
Read:
string(2) "23"
string(2) "23"
Property:
Isset:
bool(true)
bool(true)
Empty:
bool(false)
bool(false)
Null coalesce:
string(2) "23"
string(2) "23"
Read:
string(2) "23"
string(2) "23"
Offset: 'name'
Dimension:
Isset:
bool(true)
bool(true)
Empty:
bool(true)
bool(true)
Null coalesce:
string(0) ""
string(0) ""
Read:
string(0) ""
string(0) ""
Property:
Isset:
bool(true)
bool(true)
Empty:
bool(true)
bool(true)
Null coalesce:
string(0) ""
string(0) ""
Read:
string(0) ""
string(0) ""
Offset: 1
Dimension:
Isset:
bool(true)
bool(true)
Empty:
bool(true)
bool(true)
Null coalesce:
string(0) ""
string(0) ""
Read:
string(0) ""
string(0) ""
Property:
Isset:
bool(true)
bool(true)
Empty:
bool(true)
bool(true)
Null coalesce:
string(0) ""
string(0) ""
Read:
string(0) ""
string(0) ""
Errors:
Cannot write to PDORow offset
Cannot append to PDORow offset

Notice: Indirect modification of overloaded element of PDORow has no effect in %s on line %d
Cannot append to PDORow offset
Cannot unset PDORow offset
Cannot write to PDORow property
Cannot unset PDORow property
