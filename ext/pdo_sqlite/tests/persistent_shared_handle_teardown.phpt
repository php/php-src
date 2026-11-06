--TEST--
Destroying a persistent PDO instance keeps handle state used by another instance
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$options = [PDO::ATTR_PERSISTENT => true];

$a = new PDO('sqlite::memory:', null, null, $options);
$b = new PDO('sqlite::memory:', null, null, $options);

$a->sqliteCreateFunction('answer', fn() => 42, 0);
$a->sqliteCreateCollation('reverse', fn($x, $y) => strcmp($y, $x));
$a->exec('CREATE TABLE test (id INTEGER UNIQUE)');
$a->exec('INSERT INTO test VALUES (1)');
try {
    $a->query('INSERT INTO test VALUES (1)');
} catch (PDOException $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

unset($a);

var_dump($b->errorCode());
var_dump($b->query('SELECT answer()')->fetchColumn());
var_dump($b->query("SELECT 'a' UNION SELECT 'b' ORDER BY 1 COLLATE reverse")->fetchAll(PDO::FETCH_COLUMN));

unset($b);

$c = new PDO('sqlite::memory:', null, null, $options);
try {
    $c->query('SELECT answer()');
} catch (PDOException $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
PDOException: SQLSTATE[23000]: Integrity constraint violation: 19 UNIQUE constraint failed: test.id
string(5) "23000"
int(42)
array(2) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "a"
}
PDOException: SQLSTATE[HY000]: General error: 1 no such function: answer
