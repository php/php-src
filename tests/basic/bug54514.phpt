--TEST--
Req #54514 (Get php binary path during script execution)
--FILE--
<?php
if(getenv('TEST_PHP_EXECUTABLE') === PHP_BINARY) {
	echo "done";
}
--EXPECT--
done
