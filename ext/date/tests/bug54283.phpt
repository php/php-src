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
--EXPECTF--
Deprecated: DatePeriod::__construct(): Passing null to parameter #1 ($start) of type string is deprecated in %s on line %d
string(24) "Unknown or bad format ()"
