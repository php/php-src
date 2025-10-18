--TEST--
PDO Common: PDO::FETCH_KEY_PAIR
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'pdo_fetch_keypair');
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR=' . __DIR__ . '/../../pdo/tests/');
}
$table = 'pdo_fetch_keypair';
require_once getenv('REDIR_TEST_DIR') . "/pdo_fetch_setup.php";

$query = "SELECT {$table}.country, {$table}.name
    FROM {$table}";
$fetchMode = \PDO::FETCH_KEY_PAIR;

print "fetch:\n";
$stmt = $db->query($query);
while (false !== ($result = $stmt->fetch($fetchMode))) {
    print_r($result);
}

print "\nfetchAll:\n";
$stmt = $db->query($query);
$result = $stmt->fetchAll($fetchMode);
print_r($result);

print "\nToo many columns:\n";
$query = "SELECT {$table}.country, {$table}.name, {$table}.userid
    FROM {$table}";
$stmt = $db->query($query);
$result = $stmt->fetchAll($fetchMode);
var_dump($result);
# ODBC does not like leaving the result unfetched
$ignore = $stmt->fetchAll(\PDO::FETCH_OBJ);

print "\nToo few columns:\n";
$query = "SELECT {$table}.country
    FROM {$table}";
$stmt = $db->query($query);
$result = $stmt->fetchAll($fetchMode);
var_dump($result);
# ODBC does not like leaving the result unfetched
$ignore = $stmt->fetchAll(\PDO::FETCH_OBJ);

print "\nSame column:\n";
$query = "SELECT {$table}.country, {$table}.country
    FROM {$table}";
$stmt = $db->query($query);
$result = $stmt->fetchAll($fetchMode);
print_r($result);

--EXPECTF--
fetch:
Array
(
    [Ukraine] => Chris
)
Array
(
    [England] => Jamie
)
Array
(
    [Germany] => Robin
)
Array
(
    [Ukraine] => Sean
)
Array
(
    [Germany] => Toni
)
Array
(
    [Germany] => Toni
)
Array
(
    [France] => Sean
)

fetchAll:
Array
(
    [Ukraine] => Sean
    [England] => Jamie
    [Germany] => Toni
    [France] => Sean
)

Too many columns:

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error: PDO::FETCH_KEY_PAIR fetch mode requires the result set to contain exactly 2 columns. in %s on line %d

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error in %s on line %d
array(0) {
}

Too few columns:

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error: PDO::FETCH_KEY_PAIR fetch mode requires the result set to contain exactly 2 columns. in %s on line %s

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error in %s on line %d
array(0) {
}

Same column:
Array
(
    [Ukraine] => Ukraine
    [England] => England
    [Germany] => Germany
    [France] => France
)
