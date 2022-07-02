--TEST--
Don't add array value type is key type is illegal
--EXTENSIONS--
opcache
--FILE--
<?php

function test(\SplObjectStorage $definitions = null) {
    $argument = new stdClass;
    $definitions[$argument] = 1;
    $definitions[$argument] += 1;
    $argument = [];
    $definitions[$argument] = 1;
    $definitions[$argument] += 1;
}
function test2() {
    $a[[]] = $undef;
}
function test3() {
    foreach (range(0, $undef) as $v) { }
}
function test4() {
    var_dump(range(0, ~$u));
}

?>
===DONE===
--EXPECT--
===DONE===
