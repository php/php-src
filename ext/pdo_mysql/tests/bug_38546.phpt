--TEST--
PDO MySQL Bug #38546 (bindParam incorrect processing of bool types)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

$db->exec("DROP TABLE IF EXISTS test");

$query = "CREATE TABLE test(
	        uid MEDIUMINT UNSIGNED NOT NULL,
            some_bool_1 BOOL NOT NULL,
            some_bool_2 BOOL NOT NULL,
            some_int TINYINT NOT NULL
		)";
$db->exec($query);

$db->exec("INSERT INTO test(uid, some_bool_1, some_bool_2, some_int) VALUES(6, 1, 1, 0)");

var_dump($db->query('SELECT * from test'));
foreach ($db->query('SELECT * from test') as $row) {
	print_r($row);
}

$st = $db->prepare("UPDATE test SET some_bool_1=?, some_bool_2=?, some_int=? WHERE uid=?");

$prefs = array(
	'uid' => 6,
	'some_bool_1' => (bool) 1,
	'some_bool_2' => (bool) 0,
	'some_int' => 1,
);

$st->bindParam(1, $prefs['some_bool_1'], PDO::PARAM_BOOL);
$st->bindParam(2, $prefs['some_bool_2'], PDO::PARAM_BOOL);
$st->bindParam(3, $prefs['some_int'], PDO::PARAM_INT);
$st->bindParam(4, $prefs['uid'], PDO::PARAM_INT);

$result = $st->execute();

if ($result === false) {
    var_dump($st->errorInfo());
} else {
    print("ok\n");
}

foreach ($db->query('SELECT * from test') as $row) {
	print_r($row);
}

$st = $db->prepare("UPDATE test SET some_bool_1=?, some_bool_2=?, some_int=? WHERE uid=?");

$prefs = array(
	'uid' => 6,
	'some_bool_1' => (bool) 0,
	'some_bool_2' => (bool) 1,
	'some_int' => 2,
);

$st->bindParam(1, $prefs['some_bool_1'], PDO::PARAM_BOOL);
$st->bindParam(2, $prefs['some_bool_2'], PDO::PARAM_BOOL);
$st->bindParam(3, $prefs['some_int'], PDO::PARAM_INT);
$st->bindParam(4, $prefs['uid'], PDO::PARAM_INT);

$result = $st->execute();

if ($result === false) {
    var_dump($st->errorInfo());
} else {
    print("ok\n");
}

foreach ($db->query('SELECT * from test') as $row) {
	print_r($row);
}

?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
object(PDOStatement)#2 (1) {
  ["queryString"]=>
  string(18) "SELECT * from test"
}
Array
(
    [uid] => 6
    [0] => 6
    [some_bool_1] => 1
    [1] => 1
    [some_bool_2] => 1
    [2] => 1
    [some_int] => 0
    [3] => 0
)
ok
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
ok
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