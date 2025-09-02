--TEST--
PDO Common: Testing PDO::FETCH_FUNC with various callables
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec('CREATE TABLE pdo_fetch_function_001(id int NOT NULL PRIMARY KEY, pl_name VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_function_001 VALUES (1, 'php')");
$db->exec("INSERT INTO pdo_fetch_function_001 VALUES (2, 'sql')");

echo "Anonymous function:\n";
$st = $db->query('SELECT * FROM pdo_fetch_function_001');
$st->fetchAll(
    PDO::FETCH_FUNC,
    function($x, $y) {
        echo $x, ' ', $y, PHP_EOL;
    }
);

echo "Internal function:\n";
$st = $db->query('SELECT pl_name FROM pdo_fetch_function_001');
var_dump($st->fetchAll(PDO::FETCH_FUNC, 'strtoupper'));

echo "Relative class callable:\n";
class foo {
    public function method($x) {
        return __METHOD__ . "($x)";
    }
}
class bar extends foo {
    public function __construct($db) {
        $st = $db->query('SELECT * FROM pdo_fetch_function_001');
        var_dump($st->fetchAll(PDO::FETCH_FUNC, [$this, 'parent::method']));
    }

    static public function factory($x, $y) {
        return __METHOD__ . "($x, $y)";
    }
}

$bar = new bar($db);

echo '["bar", "factory"] callable:', "\n";
$st = $db->query('SELECT * FROM pdo_fetch_function_001');
var_dump($st->fetchAll(PDO::FETCH_FUNC, ['bar', 'factory']));

echo '[$bar, "factory"] callable:', "\n";
$st = $db->query('SELECT * FROM pdo_fetch_function_001');
var_dump($st->fetchAll(PDO::FETCH_FUNC, [$bar, 'factory']));

echo '"bar::factory" callable:', "\n";
$st = $db->query('SELECT * FROM pdo_fetch_function_001');
var_dump($st->fetchAll(PDO::FETCH_FUNC, 'bar::factory'));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_001");
?>
--EXPECTF--
Anonymous function:
1 php
2 sql
Internal function:
array(2) {
  [0]=>
  string(3) "PHP"
  [1]=>
  string(3) "SQL"
}
Relative class callable:

Deprecated: Callables of the form ["bar", "parent::method"] are deprecated in %s on line %d
array(2) {
  [0]=>
  string(14) "foo::method(1)"
  [1]=>
  string(14) "foo::method(2)"
}
["bar", "factory"] callable:
array(2) {
  [0]=>
  string(20) "bar::factory(1, php)"
  [1]=>
  string(20) "bar::factory(2, sql)"
}
[$bar, "factory"] callable:
array(2) {
  [0]=>
  string(20) "bar::factory(1, php)"
  [1]=>
  string(20) "bar::factory(2, sql)"
}
"bar::factory" callable:
array(2) {
  [0]=>
  string(20) "bar::factory(1, php)"
  [1]=>
  string(20) "bar::factory(2, sql)"
}
