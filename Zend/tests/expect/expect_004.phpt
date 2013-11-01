--TEST--
test providing reason (fail)
--INI--
zend.expectations=1
--FILE--
<?php
try {
    expect false : "I require this to succeed";
} catch (ExpectationException $ex) {
    var_dump($ex->getMessage());
}
?>
--EXPECTF--	
string(25) "I require this to succeed"

