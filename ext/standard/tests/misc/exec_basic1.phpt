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
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(system($cmd, $output));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(passthru($cmd, $output));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
exec(): Argument #1 ($command) must not contain any null bytes
system(): Argument #1 ($command) must not contain any null bytes
passthru(): Argument #1 ($command) must not contain any null bytes
