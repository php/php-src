--TEST--
to_string()
--FILE--
<?php

// valid
var_dump(to_string("foobar"));
var_dump(to_string(123));
var_dump(to_string(123.45));
var_dump(to_string(INF));
var_dump(to_string(-INF));
var_dump(to_string(NAN));

echo PHP_EOL;

// invalid
var_dump(to_string(NULL));
var_dump(to_string(TRUE));
var_dump(to_string(FALSE));
var_dump(to_string([]));
var_dump(to_string(fopen('data:text/plan,foobar', 'r')));

echo PHP_EOL;

class NotStringable {}
class Stringable {
	public function __toString() {
		return "foobar";
	}
}

// objects
var_dump(to_string(new StdClass));
var_dump(to_string(new NotStringable));
var_dump(to_string(new Stringable));

--EXPECT--
string(6) "foobar"
string(3) "123"
string(6) "123.45"
string(3) "INF"
string(4) "-INF"
string(3) "NAN"

NULL
NULL
NULL
NULL
NULL

NULL
NULL
string(6) "foobar"
