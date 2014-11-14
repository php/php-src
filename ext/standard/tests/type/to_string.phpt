--TEST--
to_string()
--FILE--
<?php

function to_string_wrapper($value) {
    try {
        return to_string($value);
    } catch (CastException $e) {
        return NULL;
    }
}

// valid
var_dump(to_string_wrapper("foobar"));
var_dump(to_string_wrapper(123));
var_dump(to_string_wrapper(123.45));
var_dump(to_string_wrapper(INF));
var_dump(to_string_wrapper(-INF));
var_dump(to_string_wrapper(NAN));
var_dump(to_string_wrapper(""));

echo PHP_EOL;

// invalid
var_dump(to_string_wrapper(NULL));
var_dump(to_string_wrapper(TRUE));
var_dump(to_string_wrapper(FALSE));
var_dump(to_string_wrapper([]));
var_dump(to_string_wrapper(fopen('data:text/plan,foobar', 'r')));

echo PHP_EOL;

class NotStringable {}
class Stringable {
	public function __toString() {
		return "foobar";
	}
}

// objects
var_dump(to_string_wrapper(new StdClass));
var_dump(to_string_wrapper(new NotStringable));
var_dump(to_string_wrapper(new Stringable));

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
