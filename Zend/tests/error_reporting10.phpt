--TEST--
testing @ and error_reporting - 10
--FILE--
<?php

error_reporting(E_ALL & ~E_STRICT);

function make_exception()
{
	@$blah;
	str_replace();
	error_reporting(0);
	throw new Exception();
}

try {
	@make_exception();
} catch (Exception $e) {}

var_dump(error_reporting());

error_reporting(E_ALL&~E_NOTICE&~E_STRICT);

try {
	@make_exception();
} catch (Exception $e) {}

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECT--
int(14335)
int(14327)
Done
