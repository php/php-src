<?php

function f($abc) {
    return $abc + 1;
}

function test() {
    return f(?);
}

var_dump(test()(1));

?>
