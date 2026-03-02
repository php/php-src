--TEST--
pcntl_exec() - Testing error when non-stringable arguments are passed for $args and $env_vars.
--EXTENSIONS--
pcntl
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

try {
    pcntl_exec(getenv("TEST_PHP_EXECUTABLE"), ['-n'], [new stdClass()]);
} catch (Error $error) {
    echo $error->getMessage() . "\n";
}
?>
--EXPECT--
Object of class stdClass could not be converted to string
Object of class stdClass could not be converted to string
