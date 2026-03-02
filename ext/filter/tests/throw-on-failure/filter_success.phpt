--TEST--
FILTER_THROW_ON_FAILURE: successful filters do not throw
--EXTENSIONS--
filter
--GET--
a=daniel.e.scherzer@gmail.com
--FILE--
<?php

class MyString {
	public function __construct(private string $wrapped) {}
	public function __toString(): string { return $this->wrapped; }
}

echo "filter_input:\n";
var_dump(filter_input(INPUT_GET, 'a', FILTER_DEFAULT, FILTER_THROW_ON_FAILURE));
var_dump(filter_input(INPUT_GET, 'a', FILTER_DEFAULT, FILTER_REQUIRE_SCALAR | FILTER_THROW_ON_FAILURE));
var_dump(filter_input(INPUT_GET, 'a', FILTER_VALIDATE_EMAIL, FILTER_THROW_ON_FAILURE));

echo "\nfilter_input_array:\n";
var_dump(filter_input_array(INPUT_GET, ['a' => ['flags' => FILTER_REQUIRE_SCALAR | FILTER_THROW_ON_FAILURE]]));
var_dump(filter_input_array(INPUT_GET, ['a' => ['filter' => FILTER_VALIDATE_EMAIL, 'flags' => FILTER_THROW_ON_FAILURE]]));

echo "\nfilter_var:\n";
var_dump(filter_var('a', FILTER_DEFAULT, FILTER_REQUIRE_SCALAR | FILTER_THROW_ON_FAILURE));
var_dump(filter_var(new MyString('daniel.e.scherzer@gmail.com'), FILTER_VALIDATE_EMAIL, FILTER_THROW_ON_FAILURE));
var_dump(filter_var('daniel.e.scherzer@gmail.com', FILTER_VALIDATE_EMAIL, FILTER_THROW_ON_FAILURE));

echo "\nfilter_var_array:\n";
var_dump(filter_var_array(['a' => 'a'], ['a' => ['flags' => FILTER_REQUIRE_SCALAR | FILTER_THROW_ON_FAILURE]]));
var_dump(filter_var_array(['a' => new MyString('bar')], ['a' => ['flags' => FILTER_THROW_ON_FAILURE]]));
var_dump(filter_var_array(['a' => 'daniel.e.scherzer@gmail.com'], ['a' => ['filter' => FILTER_VALIDATE_EMAIL, 'flags' => FILTER_THROW_ON_FAILURE]]));

?>
--EXPECT--
filter_input:
string(27) "daniel.e.scherzer@gmail.com"
string(27) "daniel.e.scherzer@gmail.com"
string(27) "daniel.e.scherzer@gmail.com"

filter_input_array:
array(1) {
  ["a"]=>
  string(27) "daniel.e.scherzer@gmail.com"
}
array(1) {
  ["a"]=>
  string(27) "daniel.e.scherzer@gmail.com"
}

filter_var:
string(1) "a"
string(27) "daniel.e.scherzer@gmail.com"
string(27) "daniel.e.scherzer@gmail.com"

filter_var_array:
array(1) {
  ["a"]=>
  string(1) "a"
}
array(1) {
  ["a"]=>
  string(3) "bar"
}
array(1) {
  ["a"]=>
  string(27) "daniel.e.scherzer@gmail.com"
}
