--TEST--
UUID::v5
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump(UUID::v5(UUID::NamespaceDNS(), 'php.net')->toString());

?>
--EXPECT--
string(36) "c4a760a8-dbcf-5254-a0d9-6a4474bd1b62"
