--TEST--
Testing const names
--FILE--
<?php

const a = 'a';
const A = 'b';


class a {
    const a = 'c';
    const A = 'd';
}

var_dump(a, A, a::a, a::A);

?>
--EXPECT--
string(1) "a"
string(1) "b"
string(1) "c"
string(1) "d"
