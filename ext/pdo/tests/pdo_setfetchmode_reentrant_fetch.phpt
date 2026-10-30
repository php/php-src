--TEST--
PDO Common: setFetchMode autoload re-entry observes the prior complete mode
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if ($dir === false) {
    die('skip no driver');
}
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR=' . __DIR__ . '/../../pdo/tests/');
}
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$pdo = PDOTest::factory();

class PriorFetchClass
{
    public int $i;

    public function __construct(public string $marker)
    {
    }
}

$pdo->exec('CREATE TABLE pdo_setfetchmode_reentrant_fetch (i INT)');
$pdo->exec('INSERT INTO pdo_setfetchmode_reentrant_fetch (i) VALUES (1)');
$pdo->exec('INSERT INTO pdo_setfetchmode_reentrant_fetch (i) VALUES (2)');

$stmt = $pdo->query('SELECT i FROM pdo_setfetchmode_reentrant_fetch ORDER BY i');
$into = new stdClass;
$stmt->setFetchMode(PDO::FETCH_INTO, $into);
$autoload = function (string $class) use ($stmt, $into): void {
    $row = $stmt->fetch();
    echo "into re-entry preserved: ";
    var_dump($row === $into);
    eval("class $class { public int \$i; }");
};
spl_autoload_register($autoload);
$stmt->setFetchMode(PDO::FETCH_CLASS, 'ReenteredIntoClass');
spl_autoload_unregister($autoload);
$row = $stmt->fetch();
echo "into re-entry value: ", $into->i, "\n";
echo "outer into class: ", $row::class, " ", $row->i, "\n";

$stmt = $pdo->query('SELECT i FROM pdo_setfetchmode_reentrant_fetch ORDER BY i');
$stmt->setFetchMode(PDO::FETCH_CLASS, PriorFetchClass::class, ['kept']);
$autoload = function (string $class) use ($stmt): void {
    $row = $stmt->fetch();
    echo "class re-entry: ", $row::class, " ", $row->marker, " ", $row->i, "\n";
    eval("class $class { public int \$i; }");
};
spl_autoload_register($autoload);
$stmt->setFetchMode(PDO::FETCH_CLASS, 'ReenteredFetchClass');
spl_autoload_unregister($autoload);
$row = $stmt->fetch();
echo "outer class: ", $row::class, " ", $row->i, "\n";
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$pdo = PDOTest::factory();
PDOTest::dropTableIfExists($pdo, 'pdo_setfetchmode_reentrant_fetch');
?>
--EXPECT--
into re-entry preserved: bool(true)
into re-entry value: 1
outer into class: ReenteredIntoClass 2
class re-entry: PriorFetchClass kept 1
outer class: ReenteredFetchClass 2
