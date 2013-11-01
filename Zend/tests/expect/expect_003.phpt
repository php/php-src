--TEST--
test catching failed expectation
--INI--
zend.expectations=1
--FILE--
<?php
try {
    expect false;
} catch (ExpectationException $ex) {
    var_dump($ex->getMessage());
}
?>
--EXPECT--	
string(12) "expect false"
