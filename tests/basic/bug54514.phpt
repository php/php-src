--TEST--
Req #54514 (Get php binary path during script execution)
--FILE--
<?php
if(realpath(getenv('TEST_PHP_EXECUTABLE')) === realpath(PHP_BINARY)) {
	echo "done";
} else {
	var_dump(getenv('TEST_PHP_EXECUTABLE'));
	var_dump(PHP_BINARY);
}
--EXPECT--
done
