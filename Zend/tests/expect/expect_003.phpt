--TEST--
test catching failed expectation
--SKIPIF--
<?php
if (!ini_get("zend.expectations"))
    die("skip: expectations disabled");
?>
--FILE--
<?php
try {
    expect false;
} catch (ExpectationException $ex) {
    var_dump($ex->getMessage());
}
?>
--EXPECTF--	
string(12) "expect false"

