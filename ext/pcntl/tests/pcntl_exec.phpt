--TEST--
pcntl_exec()
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!getenv("TEST_PHP_EXECUTABLE") || !is_executable(getenv("TEST_PHP_EXECUTABLE"))) die("skip TEST_PHP_EXECUTABLE not set");
?>
--FILE--
<?php
echo "ok\n";
pcntl_exec(getenv("TEST_PHP_EXECUTABLE"), ['-n']);
echo "nok\n";
?>
--EXPECT--
ok
