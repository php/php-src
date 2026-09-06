--TEST--
Bug #71882 (Negative ftruncate() on php://memory exhausts memory)
--FILE--
<?php
$fd = fopen("php://memory", "w+");
try {
    var_dump(ftruncate($fd, -1));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: ftruncate(): Argument #2 ($size) must be greater than or equal to 0
