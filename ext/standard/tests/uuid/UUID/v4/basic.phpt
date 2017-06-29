--TEST--
UUID::v4
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

var_dump(UUID::v4() != UUID::v4());

?>
--EXPECT--
bool(true)
