--TEST--
Pipe assign operator with all variable target types
--FILE--
<?php

$x = "hello";
$x |>= strtoupper(...);
var_dump($x);

$data = ["key" => "hello"];
$data["key"] |>= strtoupper(...);
var_dump($data["key"]);

$nested = ["a" => ["b" => "hello"]];
$nested["a"]["b"] |>= strtoupper(...);
var_dump($nested["a"]["b"]);

$obj = new stdClass;
$obj->name = "hello";
$obj->name |>= strtoupper(...);
var_dump($obj->name);

class Foo {
    public static string $val = "hello";
}
Foo::$val |>= strtoupper(...);
var_dump(Foo::$val);

?>
--EXPECT--
string(5) "HELLO"
string(5) "HELLO"
string(5) "HELLO"
string(5) "HELLO"
string(5) "HELLO"
