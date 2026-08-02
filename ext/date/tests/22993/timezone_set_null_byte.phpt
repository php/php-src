--TEST--
GH-22993: date_default_timezone_set() notice with embedded NUL byte
--FILE--
<?php

date_default_timezone_set("foo\0bar");

?>
--EXPECTF--
Notice: date_default_timezone_set(): Timezone ID 'foo%0bar' is invalid in %s on line %d
