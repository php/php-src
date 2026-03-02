--TEST--
Bug #72854: PHP Crashes on duplicate destructor call
--FILE--
<?php

function get() {
    $t = new stdClass;
    $t->prop = $t;
    return $t;
}

$i = 42;
get()->prop =& $i;

?>
===DONE===
--EXPECT--
===DONE===
