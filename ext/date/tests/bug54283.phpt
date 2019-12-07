--TEST--
Bug #54283 (new DatePeriod(NULL) causes crash)
--FILE--
<?php

try {
	var_dump(new DatePeriod(NULL));
} catch (\Error $e) {
	var_dump($e->getMessage());
}

?>
--EXPECT--
string(49) "The ISO interval '' did not contain a start date."
