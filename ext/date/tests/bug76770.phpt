--TEST--
Bug #76770 'U' modifier in 'datetime::createFromFormat' adds seconds to other specifiers
--FILE--
<?php
var_dump(DateTime::createFromFormat('U H', '3600 01')->getTimestamp());
?>
--EXPECT--
int(3600)
