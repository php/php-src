--TEST--
Validation of attribute repetition (is allowed; internal attribute)
--EXTENSIONS--
zend_test
--FILE--
<?php

#[ZendTestRepeatableAttribute]
#[ZendTestRepeatableAttribute]
const MY_CONST = true;

echo "Done\n";

?>
--EXPECT--
Done
