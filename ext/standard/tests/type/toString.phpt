--TEST--
toString()
--FILE--
<?php

// valid
var_dump(toString("foobar"));
var_dump(toString(123));
var_dump(toString(123.45));
var_dump(toString(INF));
var_dump(toString(-INF));
var_dump(toString(NAN));

echo PHP_EOL;

// invalid
var_dump(toString(NULL));
var_dump(toString(TRUE));
var_dump(toString(FALSE));
var_dump(toString([]));
var_dump(toString(fopen('data:text/plan,foobar', 'r')));

echo PHP_EOL;

class NotStringable {}
class Stringable {
	public function __toString() {
		return "foobar";
	}
}

// objects
var_dump(toString(new StdClass));
var_dump(toString(new NotStringable));
var_dump(toString(new Stringable));

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
