--TEST--
Test is_list() function
--FILE--
<?php

function test_is_list(string $desc, $val) : void {
	printf("%s: %s\n", $desc, is_list($val) ? "true" : "false");
}

test_is_list("empty", []);
test_is_list("one", [1]);
test_is_list("two", [1,2]);
test_is_list("three", [1,2,3]);
test_is_list("four", [1,2,3,4]);
test_is_list("ten", range(0, 10));

test_is_list("null", null);
test_is_list("int", 123);
test_is_list("float", 1.23);
test_is_list("string", "string");
test_is_list("object", new stdclass);
test_is_list("true", true);
test_is_list("false", false);

test_is_list("string key", ["a" => 1]);
test_is_list("mixed keys", [0 => 0, "a" => 1]);
test_is_list("ordered keys", [0 => 0, 1 => 1]);
test_is_list("shuffled keys", [1 => 0, 0 => 1]);
test_is_list("skipped keys", [0 => 0, 2 => 2]);

$arr = [1, 2, 3];
unset($arr[0]);
test_is_list("unset first", $arr);

$arr = [1, 2, 3];
unset($arr[1]);
test_is_list("unset middle", $arr);

$arr = [1, 2, 3];
unset($arr[2]);
test_is_list("unset end", $arr);

$arr = [1, "a" => "a", 2];
unset($arr["a"]);
test_is_list("unset string key", $arr);

$arr = [1 => 1, 0 => 0];
unset($arr[1]);
test_is_list("unset into order", $arr);

$arr = ["a" => 1];
unset($arr["a"]);
test_is_list("unset to empty", $arr);

$arr = [1, 2, 3];
$arr[] = 4;
test_is_list("append implicit", $arr);

$arr = [1, 2, 3];
$arr[3] = 4;
test_is_list("append explicit", $arr);

$arr = [1, 2, 3];
$arr[4] = 5;
test_is_list("append with gap", $arr);

--EXPECT--
empty: true
one: true
two: true
three: true
four: true
ten: true
null: false
int: false
float: false
string: false
object: false
true: false
false: false
string key: false
mixed keys: false
ordered keys: true
shuffled keys: false
skipped keys: false
unset first: false
unset middle: false
unset end: true
unset string key: true
unset into order: true
unset to empty: true
append implicit: true
append explicit: true
append with gap: false
