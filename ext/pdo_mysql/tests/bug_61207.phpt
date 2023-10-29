--TEST--
PDO MySQL Bug #61207 (PDO::nextRowset() after a multi-statement query doesn't always work)
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
/** @var PDO $db */
$db = MySQLPDOTest::factory();

$db->query('DROP TABLE IF EXISTS test');
$db->query('create table `test`( `id` int )');

$handle1 = $db->prepare('insert into test(id) values(1);
                          select * from test where id = ?;
                          update test set id = 2 where id = ?;');

$handle1->bindValue(1, '1');
$handle1->bindValue(2, '1');

$handle1->execute();
$i = 1;
print("Handle 1:\n");
do {
    print('Rowset ' . $i++ . "\n");
    if ($handle1->columnCount() > 0)
        print("Results detected\n");
} while($handle1->nextRowset());

$handle2 = $db->prepare('select * from test where id = ?;
                           update test set id = 1 where id = ?;');

$handle2->bindValue(1, '2');
$handle2->bindValue(2, '2');

$handle2->execute();

$i = 1;
print("Handle 2:\n");
do {
    print('Rowset ' . $i++ . "\n");
    if ($handle2->columnCount() > 0)
        print("Results detected\n");
} while($handle2->nextRowset());

$handle3 = $db->prepare('update test set id = 2 where id = ?;
                           select * from test where id = ?;');

$handle3->bindValue(1, '1');
$handle3->bindValue(2, '2');

$handle3->execute();

$i = 1;
print("Handle 3:\n");
do {
    print('Rowset ' . $i++ . "\n");
    if ($handle3->columnCount() > 0)
        print("Results detected\n");
} while($handle3->nextRowset());

$handle4 = $db->prepare('insert into test(id) values(3);
                           update test set id = 2 where id = ?;
                           select * from test where id = ?;');

$handle4->bindValue(1, '3');
$handle4->bindValue(2, '2');

$handle4->execute();

$i = 1;
print("Handle 4:\n");
do {
    print('Rowset ' . $i++ . "\n");
    if ($handle1->columnCount() > 0)
        print("Results detected\n");
} while($handle1->nextRowset());

$db->query("DROP TABLE test");
?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
Handle 1:
Rowset 1
Rowset 2
Results detected
Rowset 3
Handle 2:
Rowset 1
Results detected
Rowset 2
Handle 3:
Rowset 1
Rowset 2
Results detected
Handle 4:
Rowset 1
Rowset 2
Rowset 3
Results detected
