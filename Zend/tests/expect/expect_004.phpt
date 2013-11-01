--TEST--
test providing reason (fail)
--SKIPIF--
<?php
if (!ini_get("zend.expectations"))
    die("skip: expectations disabled");
?>
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

