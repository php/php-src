--TEST--
UUID::isNil
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

var_dump(UUID::Nil()->isNil());

?>
--EXPECT--
bool(true)
