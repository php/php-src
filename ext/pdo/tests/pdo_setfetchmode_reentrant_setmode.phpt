--TEST--
PDO Common: setFetchMode autoload re-entry may change the statement safely
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

$pdo->exec('CREATE TABLE pdo_setfetchmode_reentrant_setmode (i INT)');
$pdo->exec('INSERT INTO pdo_setfetchmode_reentrant_setmode (i) VALUES (1)');
$stmt = $pdo->query('SELECT i FROM pdo_setfetchmode_reentrant_setmode');
$first = new stdClass;
$second = new stdClass;
$stmt->setFetchMode(PDO::FETCH_INTO, $first);

$autoload = function (string $class) use ($stmt, $second): void {
    $stmt->setFetchMode(PDO::FETCH_INTO, $second);
    eval("class $class { public int \$i; }");
};
spl_autoload_register($autoload);
$stmt->setFetchMode(PDO::FETCH_CLASS, 'ReenteredSetModeClass');
spl_autoload_unregister($autoload);

$row = $stmt->fetch();
echo $row::class, " ", $row->i, "\n";
echo "first changed: ";
var_dump(isset($first->i));
echo "second changed: ";
var_dump(isset($second->i));
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$pdo = PDOTest::factory();
PDOTest::dropTableIfExists($pdo, 'pdo_setfetchmode_reentrant_setmode');
?>
--EXPECT--
ReenteredSetModeClass 1
first changed: bool(false)
second changed: bool(false)
