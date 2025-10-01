--TEST--
PDO PgSQL pgsqlCopyFromArray using Generator
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->exec('CREATE TABLE test_copy_from_generator (v int)');

$generator = (function(){
    $position = 0;
    $values = [1, 1, 2, 3, 5];

    while(isset($values[$position])){
        yield $values[$position];
        ++$position;
    }
})();


$db->pgsqlCopyFromArray('test_copy_from_generator',$generator);

$stmt = $db->query("select * from test_copy_from_generator order by 1");
$result = $stmt->fetchAll(PDO::FETCH_COLUMN, 0);
var_export($result);

?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->query('DROP TABLE IF EXISTS test_copy_from_generator CASCADE');
?>
--EXPECTF--
Deprecated: Method PDO::pgsqlCopyFromArray() is deprecated since 8.5, use Pdo\Pgsql::copyFromArray() instead in %s on line %d
array (
  0 => 1,
  1 => 1,
  2 => 2,
  3 => 3,
  4 => 5,
)
