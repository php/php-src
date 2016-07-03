--TEST--
Case-sensitivity part of bug #64874 ("json_decode handles whitespace and case-sensitivity incorrectly")
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
function decode($json) {
	var_dump(json_decode($json));
	echo ((json_last_error() !== 0) ? 'ERROR' : 'SUCCESS') . PHP_EOL;
}

// Only lowercase should work
decode('true');
decode('True');
decode('[true]');
decode('[True]');
echo PHP_EOL;

decode('false');
decode('False');
decode('[false]');
decode('[False]');
echo PHP_EOL;

decode('null');
decode('Null');
decode('[null]');
decode('[Null]');
echo PHP_EOL;

echo "Done\n";
--EXPECT--
bool(true)
SUCCESS
NULL
ERROR
array(1) {
  [0]=>
  bool(true)
}
SUCCESS
NULL
ERROR

bool(false)
SUCCESS
NULL
ERROR
array(1) {
  [0]=>
  bool(false)
}
SUCCESS
NULL
ERROR

NULL
SUCCESS
NULL
ERROR
array(1) {
  [0]=>
  NULL
}
SUCCESS
NULL
ERROR

Done
