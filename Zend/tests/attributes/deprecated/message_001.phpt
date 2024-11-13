--TEST--
#[\Deprecated]: Message Formats.
--FILE--
<?php

#[\Deprecated]
function test1() {
}

#[\Deprecated()]
function test2() {
}

#[\Deprecated("use test() instead")]
function test3() {
}

#[\Deprecated(message: "use test() instead", since: "1.0")]
function test4() {
}

#[\Deprecated(since: "1.0", message: "use test() instead")]
function test5() {
}

#[\Deprecated(since: "1.0")]
function test6() {
}

test1();
test2();
test3();
test4();
test5();
test6();

?>
--EXPECTF--
Deprecated: Function test1() is deprecated in %s on line %d

Deprecated: Function test2() is deprecated in %s on line %d

Deprecated: Function test3() is deprecated, use test() instead in %s on line %d

Deprecated: Function test4() is deprecated since 1.0, use test() instead in %s on line %d

Deprecated: Function test5() is deprecated since 1.0, use test() instead in %s on line %d

Deprecated: Function test6() is deprecated since 1.0 in %s on line %d
