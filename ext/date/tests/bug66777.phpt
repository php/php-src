--TEST--
Test for bug #66777: strftime returns empty string when input fmt string exceeds 1024
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--INI--
date.timezone = UTC
--FILE--
<?php
$x = strftime(str_repeat('x', 10240));
var_dump(strlen($x));
?>
--EXPECT--
int(10240)
