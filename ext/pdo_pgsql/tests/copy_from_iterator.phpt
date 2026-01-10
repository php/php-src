--TEST--
PDO PgSQL pgsqlCopyFromArray using Iterator
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

$db->exec('CREATE TABLE test_copy_from_traversable (v int)');

$iterator = new class implements Iterator{
    private $position = 0;
    private $values = [1, 1, 2, 3, 5];

    public function rewind(): void {
        $this->position = 0;
    }

    public function current(): int {
        return $this->values[$this->position];
    }

    public function key(): int {
        return $this->position;
    }

    public function next(): void {
        ++$this->position;
    }

    public function valid(): bool {
        return isset($this->values[$this->position]);
    }
};

try {
	$db->pgsqlCopyFromArray('test_copy_from_traversable',new stdClass());
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}

$db->pgsqlCopyFromArray('test_copy_from_traversable',$iterator);

$stmt = $db->query("select * from test_copy_from_traversable order by 1");
$result = $stmt->fetchAll(PDO::FETCH_COLUMN, 0);
var_export($result);

?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->query('DROP TABLE IF EXISTS test_copy_from_traversable CASCADE');
?>
--EXPECTF--
Deprecated: Method PDO::pgsqlCopyFromArray() is deprecated since 8.5, use Pdo\Pgsql::copyFromArray() instead in %s on line %d
PDO::pgsqlCopyFromArray(): Argument #2 ($rows) must be of type Traversable|array, stdClass given

Deprecated: Method PDO::pgsqlCopyFromArray() is deprecated since 8.5, use Pdo\Pgsql::copyFromArray() instead in %s on line %d
array (
  0 => 1,
  1 => 1,
  2 => 2,
  3 => 3,
  4 => 5,
)
