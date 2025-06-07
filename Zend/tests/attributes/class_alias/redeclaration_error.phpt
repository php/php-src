--TEST--
Cannot redeclare an existing class
--FILE--
<?php

#[ClassAlias('Attribute')]
class Demo {}

?>
--EXPECTF--
Fatal error: Unable to declare alias 'Attribute' for 'Demo' in %s on line %d
