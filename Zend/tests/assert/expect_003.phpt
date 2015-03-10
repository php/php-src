--TEST--
test catching failed assertion
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php
try {
    assert(false);
} catch (AssertionException $ex) {
    var_dump($ex->getMessage());
}
?>
--EXPECT--
string(13) "assert(false)"
