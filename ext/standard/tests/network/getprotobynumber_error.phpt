--TEST--
getprotobynumber function errors test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
// invalid protocol number
var_dump(getprotobynumber(999));
?>
--EXPECT--
bool(false)
