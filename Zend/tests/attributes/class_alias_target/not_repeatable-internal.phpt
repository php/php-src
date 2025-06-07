--TEST--
Validation of attribute repetition (not allowed; internal attribute)
--FILE--
<?php

#[ClassAlias('Other', [new Deprecated(), new Deprecated()])]
class Demo {}

?>
--EXPECT--

e