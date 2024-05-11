--TEST--
Test PdoSqlite::createAggregate() trampoline callback
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PdoSqlite('sqlite::memory:');

$db->query('CREATE TABLE test_pdo_sqlite_createaggregate_trampoline (a INTEGER, b INTEGER)');

$stmt = $db->query('INSERT INTO test_pdo_sqlite_createaggregate_trampoline VALUES (1, -1), (2, -2), (3, -3), (4, -4), (4, -4)');

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        $context = $arguments[0];
        if ($name === 'finalize') {
            return implode(',', $context['values']);
        }
        if (empty($context)) {
            $context = ['total' => 0, 'values' => []];
        }
        $context['total'] += (int) $arguments[2];
        $context['values'][] = $context['total'];
        return $context;
    }
}

var_dump($db->createAggregate('S', [new TrampolineTest(), 'step'], [new TrampolineTest(), 'finalize'], 1));

foreach ($db->query('SELECT S(a), S(b) FROM test_pdo_sqlite_createaggregate_trampoline') as $row) {
    var_dump($row);
}

?>
--EXPECT--
bool(true)
Trampoline for step
Trampoline for step
Trampoline for step
Trampoline for step
Trampoline for step
Trampoline for step
Trampoline for step
Trampoline for step
Trampoline for step
Trampoline for step
Trampoline for finalize
Trampoline for finalize
array(4) {
  ["S(a)"]=>
  string(11) "1,3,6,10,14"
  [0]=>
  string(11) "1,3,6,10,14"
  ["S(b)"]=>
  string(16) "-1,-3,-6,-10,-14"
  [1]=>
  string(16) "-1,-3,-6,-10,-14"
}
