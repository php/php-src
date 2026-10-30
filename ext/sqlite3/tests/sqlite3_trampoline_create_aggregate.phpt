--TEST--
SQLite3::createAggregate() trampoline callback
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

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
$o = new TrampolineTest();
$step = [$o, 'step'];
$finalize = [$o, 'finalize'];

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (a INTEGER, b INTEGER)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO test (a, b) VALUES (1, -1)"));
var_dump($db->exec("INSERT INTO test (a, b) VALUES (2, -2)"));
var_dump($db->exec("INSERT INTO test (a, b) VALUES (3, -3)"));
var_dump($db->exec("INSERT INTO test (a, b) VALUES (4, -4)"));
var_dump($db->exec("INSERT INTO test (a, b) VALUES (4, -4)"));

$db->createAggregate('S', $step, $finalize, 1);

print_r($db->querySingle("SELECT S(a), S(b) FROM test", true));

echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECT--
Creating Table
bool(true)
INSERT into table
bool(true)
bool(true)
bool(true)
bool(true)
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
Array
(
    [S(a)] => 1,3,6,10,14
    [S(b)] => -1,-3,-6,-10,-14
)
Closing database
bool(true)
Done
