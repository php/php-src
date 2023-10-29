--TEST--
PDO MySQL Bug #38546 (bindParam incorrect processing of bool types)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

// To test error cases.
$db->exec("SET sql_mode='STRICT_TRANS_TABLES'");

$db->exec("DROP TABLE IF EXISTS test");

$query = "CREATE TABLE test(
            uid MEDIUMINT UNSIGNED NOT NULL,
            some_bool_1 BOOL NOT NULL,
            some_bool_2 BOOL NOT NULL,
            some_int TINYINT NOT NULL
        )";
$db->exec($query);

$st = $db->prepare("INSERT INTO test (uid, some_bool_1, some_bool_2, some_int) VALUES (?, ?, ?, ?)");

$values = [
    'uid' => 6,
    'some_bool_1' => false,
    'some_bool_2' => true,
    'some_int' => -23
];
$st->bindParam(1, $values['uid'], PDO::PARAM_INT);
$st->bindParam(2, $values['some_bool_1'], PDO::PARAM_BOOL);
$st->bindParam(3, $values['some_bool_2'], PDO::PARAM_BOOL);
$st->bindParam(4, $values['some_int'], PDO::PARAM_INT);

$result = $st->execute();

if ($result === false) {
    var_dump($st->errorInfo());
} else {
    print("ok insert\n");
}

foreach ($db->query('SELECT * from test') as $row) {
    print_r($row);
}

$st = $db->prepare("UPDATE test SET some_bool_1=?, some_bool_2=?, some_int=? WHERE uid=?");

$values = [
    'uid' => 6,
    'some_bool_1' => (bool) 1,
    'some_bool_2' => (bool) 0,
    'some_int' => 1,
];

$st->bindParam(1, $values['some_bool_1'], PDO::PARAM_BOOL);
$st->bindParam(2, $values['some_bool_2'], PDO::PARAM_BOOL);
$st->bindParam(3, $values['some_int'], PDO::PARAM_INT);
$st->bindParam(4, $values['uid'], PDO::PARAM_INT);

$result = $st->execute();

if ($result === false) {
    var_dump($st->errorInfo());
} else {
    print("ok prepare 1\n");
}

foreach ($db->query('SELECT * from test') as $row) {
    print_r($row);
}

$st = $db->prepare("UPDATE test SET some_bool_1=?, some_bool_2=?, some_int=? WHERE uid=?");

$values = [
    'uid' => 6,
    'some_bool_1' => (bool) 0,
    'some_bool_2' => (bool) 1,
    'some_int' => 2,
];

$st->bindParam(1, $values['some_bool_1'], PDO::PARAM_BOOL);
$st->bindParam(2, $values['some_bool_2'], PDO::PARAM_BOOL);
$st->bindParam(3, $values['some_int'], PDO::PARAM_INT);
$st->bindParam(4, $values['uid'], PDO::PARAM_INT);

$result = $st->execute();

if ($result === false) {
    var_dump($st->errorInfo());
} else {
    print("ok prepare 2\n");
}

foreach ($db->query('SELECT * from test') as $row) {
    print_r($row);
}

// String true and false should fail
$st = $db->prepare("UPDATE test SET some_bool_1=?, some_bool_2=?, some_int=? WHERE uid=?");

$values = [
    'uid' => 6,
    'some_bool_1' => 'true',
    'some_bool_2' => 'false',
    'some_int' => 3,
];

$st->bindParam(1, $values['some_bool_1'], PDO::PARAM_BOOL);
$st->bindParam(2, $values['some_bool_2'], PDO::PARAM_BOOL);
$st->bindParam(3, $values['some_int'], PDO::PARAM_INT);
$st->bindParam(4, $values['uid'], PDO::PARAM_INT);

$result = $st->execute();

if ($result === false) {
    var_dump($st->errorInfo());
} else {
    print("ok prepare 3\n");
}

foreach ($db->query('SELECT * from test') as $row) {
    print_r($row);
}

// Null should not be treated as false
$st = $db->prepare("UPDATE test SET some_bool_1=?, some_bool_2=?, some_int=? WHERE uid=?");

$values = [
    'uid' => 6,
    'some_bool_1' => true,
    'some_bool_2' => null,
    'some_int' => 4,
];

$st->bindParam(1, $values['some_bool_1'], PDO::PARAM_BOOL);
$st->bindParam(2, $values['some_bool_2'], PDO::PARAM_BOOL);
$st->bindParam(3, $values['some_int'], PDO::PARAM_INT);
$st->bindParam(4, $values['uid'], PDO::PARAM_INT);

$result = $st->execute();

if ($result === false) {
    var_dump($st->errorInfo());
} else {
    print("ok prepare 4\n");
}

foreach ($db->query('SELECT * from test') as $row) {
    print_r($row);
}

// Integers converted correctly
$st = $db->prepare("UPDATE test SET some_bool_1=?, some_bool_2=?, some_int=? WHERE uid=?");

$values = [
    'uid' => 6,
    'some_bool_1' => 256,
    'some_bool_2' => 0,
    'some_int' => 5,
];

$st->bindParam(1, $values['some_bool_1'], PDO::PARAM_BOOL);
$st->bindParam(2, $values['some_bool_2'], PDO::PARAM_BOOL);
$st->bindParam(3, $values['some_int'], PDO::PARAM_INT);
$st->bindParam(4, $values['uid'], PDO::PARAM_INT);

$result = $st->execute();

if ($result === false) {
    var_dump($st->errorInfo());
} else {
    print("ok prepare 5\n");
}

foreach ($db->query('SELECT * from test') as $row) {
    print_r($row);
}

?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
ok insert
Array
(
    [uid] => 6
    [0] => 6
    [some_bool_1] => 0
    [1] => 0
    [some_bool_2] => 1
    [2] => 1
    [some_int] => -23
    [3] => -23
)
ok prepare 1
Array
(
    [uid] => 6
    [0] => 6
    [some_bool_1] => 1
    [1] => 1
    [some_bool_2] => 0
    [2] => 0
    [some_int] => 1
    [3] => 1
)
ok prepare 2
Array
(
    [uid] => 6
    [0] => 6
    [some_bool_1] => 0
    [1] => 0
    [some_bool_2] => 1
    [2] => 1
    [some_int] => 2
    [3] => 2
)

Warning: PDOStatement::execute(): SQLSTATE[%s]: %s: 1366 Incorrect integer value: 'true' for column %s at row 1 in %s
array(3) {
  [0]=>
  string(5) "%s"
  [1]=>
  int(1366)
  [2]=>
  string(%d) "Incorrect integer value: 'true' for column %s at row 1"
}
Array
(
    [uid] => 6
    [0] => 6
    [some_bool_1] => 0
    [1] => 0
    [some_bool_2] => 1
    [2] => 1
    [some_int] => 2
    [3] => 2
)

Warning: PDOStatement::execute(): SQLSTATE[23000]: Integrity constraint violation: 1048 Column 'some_bool_2' cannot be null in %s
array(3) {
  [0]=>
  string(5) "23000"
  [1]=>
  int(1048)
  [2]=>
  string(35) "Column 'some_bool_2' cannot be null"
}
Array
(
    [uid] => 6
    [0] => 6
    [some_bool_1] => 0
    [1] => 0
    [some_bool_2] => 1
    [2] => 1
    [some_int] => 2
    [3] => 2
)
ok prepare 5
Array
(
    [uid] => 6
    [0] => 6
    [some_bool_1] => 1
    [1] => 1
    [some_bool_2] => 0
    [2] => 0
    [some_int] => 5
    [3] => 5
)
