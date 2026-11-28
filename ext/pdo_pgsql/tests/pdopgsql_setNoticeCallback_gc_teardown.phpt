--TEST--
Pdo\Pgsql notice callback freed by the garbage collector before its connection
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
$db = PDOTest::factory(Pdo\Pgsql::class);
try {
    $db->exec("SET log_statement = 'all'");
} catch (PDOException $e) {
    die("skip cannot set log_statement");
}
?>
--FILE--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';

class NoticeHandler {
    public string $prefix;

    public function __construct(public PDO $db) {
        $this->prefix = str_repeat('-', 2);
    }

    public function __invoke(string $message): void {
        echo $this->prefix, " ", trim($message), "\n";
    }
}

$db = PDOTest::factory(Pdo\Pgsql::class);
$db->exec("SET log_statement = 'all'");
$db->exec("SET client_min_messages = log");
$handler = new NoticeHandler($db);
$db->setNoticeCallback($handler);
$db->beginTransaction();
gc_collect_cycles();
unset($handler, $db);
var_dump(gc_collect_cycles() > 0);
?>
--EXPECT--
-- LOG:  statement: BEGIN
bool(true)
