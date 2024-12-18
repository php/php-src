--TEST--
Bug GH-16944 (Invalid filtering of IPv6 with FILTER_FLAG_NO_RES_RANGE)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var("::ffff:0:1", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
?>
--EXPECT--
bool(false)
