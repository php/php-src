--TEST--
PDO PgSQL failed persistent connection does not crash on object destruction
--EXTENSIONS--
pdo_pgsql
--FILE--
<?php

try {
    new Pdo\Pgsql('pgsql:host=/nonexistent', options: [
        PDO::ATTR_PERSISTENT => true,
    ]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Failed connection object destroyed without crash\n";

?>
--EXPECTF--
PDOException: SQLSTATE[08006] [7] %a
Failed connection object destroyed without crash
