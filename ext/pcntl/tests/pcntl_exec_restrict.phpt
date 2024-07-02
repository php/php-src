--TEST--
pcntl_exec() - Testing with open_basedir restriction.
--EXTENSIONS--
pcntl
--INI--
open_basedir=${HOME}/bin
--SKIPIF--
<?php
if (!getenv("TEST_PHP_EXECUTABLE") || !is_executable(getenv("TEST_PHP_EXECUTABLE"))) die("skip TEST_PHP_EXECUTABLE not set");
?>
--FILE--
<?php
try {
    pcntl_exec(getenv("TEST_PHP_EXECUTABLE"), ['-n', new stdClass()]);
} catch (Error $error) {
    echo $error->getMessage() . "\n";
}

?>
--EXPECT--
pcntl_exec(): Argument #1 ($path) open_basedir restriction in effect.
