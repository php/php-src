--TEST--
test providing reason (pass)
--SKIPIF--
<?php
if (!ini_get("zend.expectations"))
    die("skip: expectations disabled");
?>
--FILE--
<?php
try {
    /* by passing we test there are no leaks upon success */
    expect true : "I require this to succeed";
} catch (ExpectationException $ex) {
    var_dump($ex->getMessage());
}
var_dump(true);
?>
--EXPECTF--	
bool(true)

