--TEST--
Test preg_match() function : error conditions - Internal error
--FILE--
<?php

try {
    preg_match('/', 'Hello world');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(preg_last_error_msg() === 'Internal error');

?>
--EXPECT--
preg_match(): Regular expression doesn't contain an ending delimiter "/"
bool(true)
