--TEST--
pcntl_exec() 2
--SKIPIF--
<?php if (!getenv("TEST_PHP_EXECUTABLE") || !is_executable(getenv("TEST_PHP_EXECUTABLE"))) die("skip TEST_PHP_EXECUTABLE not set"); ?>
--FILE--
<?php
if (getenv("PCNTL_EXEC_TEST_IS_CHILD")) {
	var_dump((binary)getenv("FOO"));
	exit;
}
echo "ok\n";
pcntl_exec(getenv("TEST_PHP_EXECUTABLE"), array(__FILE__), array(
	b"PCNTL_EXEC_TEST_IS_CHILD" => b"1", 
	b"FOO" => b"BAR",
	1 => b"long")
);

echo "nok\n";
?>
--EXPECT--
ok
string(3) "BAR"
