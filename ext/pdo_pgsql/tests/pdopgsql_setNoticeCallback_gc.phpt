--TEST--
Pdo\Pgsql notice callback cycles are collected
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

class NoticeHandler {
    public function __construct(public PDO $db) {}

    public function handle(string $message): void {
        echo trim($message), "\n";
    }

    public function __invoke(string $message): void {
        $this->handle($message);
    }
}

foreach (['closure', 'method', 'invokable'] as $kind) {
    $db = PDOTest::factory(Pdo\Pgsql::class);
    $callback = match ($kind) {
        'closure' => function ($message) use ($db) {
            echo trim($message), "\n";
        },
        'method' => [new NoticeHandler($db), 'handle'],
        'invokable' => new NoticeHandler($db),
    };
    $db->setNoticeCallback($callback);
    $weak = WeakReference::create($db);
    unset($callback);
    gc_collect_cycles();
    $db->exec("DO $$ BEGIN RAISE NOTICE '$kind'; END $$");
    unset($db);
    gc_collect_cycles();
    echo "$kind collected: ";
    var_dump($weak->get() === null);
}
?>
--EXPECT--
NOTICE:  closure
closure collected: bool(true)
NOTICE:  method
method collected: bool(true)
NOTICE:  invokable
invokable collected: bool(true)
