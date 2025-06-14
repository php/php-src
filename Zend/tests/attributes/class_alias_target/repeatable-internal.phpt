--TEST--
Validation of attribute repetition (is allowed; internal attribute)
--EXTENSIONS--
zend_test
--FILE--
<?php

#[ClassAlias('Other', [new ZendTestRepeatableAttribute(), new ZendTestRepeatableAttribute()])]
class Demo {}

echo "Done\n";

?>
--EXPECT--
Done
