--TEST--
test looping expect (pass)
--SKIPIF--
<?php
if (!ini_get("zend.expectations"))
    die("skip: expectations disabled");
?>
--FILE--
<?php
for($i=0; $i<100000; $i++) {
    /* test looping expect */
    expect ($i < 100000) :
        "The universe should make sense";
}
var_dump(true);
?>
--EXPECTF--	
bool(true)

