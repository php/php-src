--TEST--
UUID::isNil
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump(UUID::Nil()->isNil());

?>
--EXPECT--
bool(true)
