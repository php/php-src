--TEST--
getprotobyname function errors test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
// invalid protocol name
var_dump(getprotobyname('abc'));
?>
--EXPECT--
bool(false)
