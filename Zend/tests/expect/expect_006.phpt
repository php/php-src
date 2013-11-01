--TEST--
test looping expect (pass)
--INI--
zend.expectations=1
--FILE--
<?php
for($i=0; $i<100000; $i++) {
    /* test looping expect */
    expect ($i < 100000) :
        "The universe should make sense";
}
var_dump(true);
?>
--EXPECT--	
bool(true)
