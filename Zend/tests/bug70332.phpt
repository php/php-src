--TEST--
Bug #70332 (Wrong behavior while returning reference on object)
--FILE--
<?php
function & test($arg) {
	return $arg;
}

$arg = new StdClass();
$arg->name = array();

test($arg)->name[1] = "xxxx";

print_r($arg);
?>
--EXPECT--
StdClass Object
(
    [name] => Array
        (
            [1] => xxxx
        )

)
