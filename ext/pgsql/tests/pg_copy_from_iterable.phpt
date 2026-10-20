--TEST--
pg_copy_from with an iterable
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = "table_copy_iter";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int)");

$iter = new class implements Iterator {
	var $count = 0;
	var $values = Array(1,2,3);

	public function next(): void {
		++$this->count;
	}

	public function rewind(): void {
		$this->count = 0;
	}

	public function current(): int {
		return $this->values[$this->count];
	}

	public function key(): int {
		return $this->count;
	}

	public function valid(): bool {
		return $this->count < count($this->values);
	}
};

try {
	pg_copy_from($db, $table_name, new stdClass());
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
var_dump(pg_copy_from($db, $table_name, $iter));
$res = pg_query($db, "SELECT FROM {$table_name}");
var_dump(count(pg_fetch_all($res)) == 3);

?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_copy_iter";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
pg_copy_from(): Argument #3 ($rows) must be of type Traversable|array, stdClass given
bool(true)
bool(true)
