--TEST--
pdo_raise_impl_error honors ERRMODE_SILENT (no warning)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$pdo = new PDO('sqlite::memory:');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
set_error_handler(function (int $errno, string $errstr): bool {
    echo "warning: $errstr\n";
    return true;
});
$result = $pdo->getAttribute(123456);
echo "result: ";
var_dump($result);
echo "errorInfo: ";
var_dump($pdo->errorInfo()[0]);
?>
--EXPECT--
result: bool(false)
errorInfo: string(5) "IM001"
