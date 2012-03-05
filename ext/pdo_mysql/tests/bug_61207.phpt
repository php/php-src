--TEST--
PDO MySQL Bug #61207 (PDO::nextRowset() after a multi-statement query doesn't always work)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';

$link = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$link->query('create table `bug61207`( `id` int )');

$handle1 = $link->prepare('insert into bug61207(id) values(1);
                          select * from bug61207 where id = ?;
                          update bug61207 set id = 2 where id = ?;');
						     
$handle1->bindValue('1', '1');
$handle1->bindValue('2', '1');
				      
$handle1->execute();   
$i = 1;
print("Handle 1:\n");
do {
	print('Rowset ' . $i++ . "\n");
	if ($handle1->columnCount() > 0)
		print("Results detected\n");
} while($handle1->nextRowset());

$handle2 = $link->prepare('select * from bug61207 where id = ?;
                           update bug61207 set id = 1 where id = ?;');
					        
$handle2->bindValue('1', '2');
$handle2->bindValue('2', '2');

$handle2->execute();

$i = 1;
print("Handle 2:\n");
do {
	print('Rowset ' . $i++ . "\n");
	if ($handle2->columnCount() > 0)
		print("Results detected\n");
} while($handle2->nextRowset());

$handle3 = $link->prepare('update bug61207 set id = 2 where id = ?;
                           select * from bug61207 where id = ?;');
 
$handle3->bindValue('1', '1');
$handle3->bindValue('2', '2');

$handle3->execute();

$i = 1;
print("Handle 3:\n");
do {
	print('Rowset ' . $i++ . "\n");
	if ($handle3->columnCount() > 0)
		print("Results detected\n");
} while($handle3->nextRowset());

$handle4 = $link->prepare('insert into bug61207(id) values(3);
                           update bug61207 set id = 2 where id = ?;
                           select * from bug61207 where id = ?;');
															      
$handle4->bindValue('1', '3');
$handle4->bindValue('2', '2');
									       
$handle4->execute();
      
$i = 1;
print("Handle 4:\n");
do {
	print('Rowset ' . $i++ . "\n");
	if ($handle1->columnCount() > 0)
		print("Results detected\n");
} while($handle1->nextRowset());

$link->query("DROP TABLE bug61207");
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
