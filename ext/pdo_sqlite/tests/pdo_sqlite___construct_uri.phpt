--TEST--
PDO_sqlite: PDO->__construct() - URI
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$dsnFile = __DIR__ . DIRECTORY_SEPARATOR . "pdo_sqlite___construct_uri.dsn";
$dbFile = __DIR__ . DIRECTORY_SEPARATOR . "pdo_sqlite___construct_uri.db";
file_put_contents($dsnFile, "sqlite:{$dbFile}");

clearstatcache();
var_dump(file_exists($dbFile));
new PDO("uri:{$dsnFile}");

clearstatcache();
var_dump(file_exists($dbFile));
unlink($dbFile);

set_error_handler(function (int $errno, string $errstr, string $errfile, int $errline) {
    throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

clearstatcache();
var_dump(file_exists($dbFile));

try {
    new PDO("uri:{$dsnFile}");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

clearstatcache();
var_dump(file_exists($dbFile));

?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "pdo_sqlite___construct_uri.dsn");
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "pdo_sqlite___construct_uri.db");
?>
--EXPECTF--
bool(false)

Deprecated: Looking up the DSN from a URI is deprecated due to possible security concerns with DSNs coming from remote URIs in %s on line %d
bool(true)
bool(false)
ErrorException: Looking up the DSN from a URI is deprecated due to possible security concerns with DSNs coming from remote URIs
bool(false)
