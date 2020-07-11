--TEST--
Bug #54283 (new DatePeriod(NULL) causes crash)
--FILE--
<?php

try {
    var_dump(new DatePeriod(NULL));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(51) "DatePeriod::__construct(): Unknown or bad format ()"
