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
unicode(1) "a"
unicode(1) "b"
unicode(1) "c"
unicode(1) "d"
