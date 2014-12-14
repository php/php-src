--TEST--
try_string()
--FILE--
<?php

// valid
var_dump(try_string("foobar"));
var_dump(try_string(123));
var_dump(try_string(123.45));
var_dump(try_string(INF));
var_dump(try_string(-INF));
var_dump(try_string(NAN));
var_dump(try_string(""));

echo PHP_EOL;

// invalid
var_dump(try_string(NULL));
var_dump(try_string(TRUE));
var_dump(try_string(FALSE));
var_dump(try_string([]));
var_dump(try_string(fopen('data:text/plan,foobar', 'r')));

echo PHP_EOL;

class NotStringable {}
class Stringable {
	public function __toString() {
		return "foobar";
	}
}

// objects
var_dump(try_string(new StdClass));
var_dump(try_string(new NotStringable));
var_dump(try_string(new Stringable));

--EXPECT--
string(6) "foobar"
string(3) "123"
string(6) "123.45"
string(3) "INF"
string(4) "-INF"
string(3) "NAN"
string(0) ""

NULL
NULL
NULL
NULL
NULL

NULL
NULL
string(6) "foobar"
