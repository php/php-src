--TEST--
A generator with an active stack can be cloned
--FILE--
<?php

function gen() {
    var_dump(str_repeat("x", yield));
}

$g1 = gen();
$g1->rewind();
$g2 = clone $g1;
unset($g1);
$g2->send(10);

?>
--EXPECT--
string(10) "xxxxxxxxxx"
