--TEST--
Error when attribute does not target class alias (internal attribute)
--FILE--
<?php

#[ClassAlias('Other', [new Override()])]
class Demo {}

?>
--EXPECTF--
Fatal error: Attribute "Override" cannot target class alias (allowed targets: method) in %s on line %d
