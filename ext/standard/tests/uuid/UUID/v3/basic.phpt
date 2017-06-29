--TEST--
UUID::v3
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

var_dump(UUID::v3(UUID::NamespaceDNS(), 'php.net')->toString());

?>
--EXPECT--
string(36) "11a38b9a-b3da-360f-9353-a5a725514269"
