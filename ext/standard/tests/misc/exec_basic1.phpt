--TEST--
exec, system, passthru  — Basic command execution functions
--SKIPIF--
<?php
// If this does not work for Windows, please uncomment or fix test
// if(substr(PHP_OS, 0, 3) == "WIN") die("skip not for Windows");
?>
--FILE--
<?php
$cmd = "echo abc\n\0command";
try {
    var_dump(exec($cmd, $output));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(system($cmd, $output));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(passthru($cmd, $output));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: exec(): Argument #1 ($command) must not contain any null bytes
ValueError: system(): Argument #1 ($command) must not contain any null bytes
ValueError: passthru(): Argument #1 ($command) must not contain any null bytes
