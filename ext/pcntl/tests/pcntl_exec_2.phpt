--TEST--
pcntl_exec() 2
--SKIPIF--
<?php

if (!extension_loaded("pcntl")) print "skip";
if (!getenv("TEST_PHP_EXECUTABLE") || !is_executable(getenv("TEST_PHP_EXECUTABLE"))) die("skip TEST_PHP_EXECUTABLE not set");

?>
--FILE--
<?php
if (getenv("PCNTL_EXEC_TEST_IS_CHILD")) {
	var_dump(getenv("FOO"));
	exit;
}
echo "ok\n";
pcntl_exec(getenv("TEST_PHP_EXECUTABLE"), array('-n', __FILE__), array(
	"PCNTL_EXEC_TEST_IS_CHILD" => "1",
	"FOO" => "BAR",
	1 => "long")
);

echo "nok\n";
?>
--EXPECT--
ok
string(3) "BAR"
