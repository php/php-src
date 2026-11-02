--TEST--
Persistent Pdo\Pgsql notice callbacks remain alive during garbage collection
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';

putenv('PDOTEST_ATTR=' . serialize([PDO::ATTR_PERSISTENT => true]));
$a = PDOTest::factory(Pdo\Pgsql::class);
$b = PDOTest::factory(Pdo\Pgsql::class);
$f = function ($message) use ($a, $b) {
    echo trim($message), "\n";
};
$a->setNoticeCallback($f);
unset($a, $b);
gc_collect_cycles();

$c = PDOTest::factory(Pdo\Pgsql::class);
$c->exec("DO $$ BEGIN RAISE NOTICE 'after gc'; END $$");
var_dump($f instanceof Closure);
$c->setNoticeCallback(null);
?>
--EXPECT--
NOTICE:  after gc
bool(true)
