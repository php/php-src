--TEST--
Pipe assign operator with partial function application
--FILE--
<?php

$x = "Hello World";
$x |>= str_replace('World', 'PHP', ?);
var_dump($x);

$x = "  trimmed  ";
$x |>= trim(?);
var_dump($x);

$x = [3, 1, 4, 1, 5];
$x |>= array_slice(?, 1, 3);
var_dump($x);

$x = "hello";
$x |>= str_pad(?, 10, '.');
var_dump($x);

$x = "hello world";
$x |>= str_replace(' ', '-', ?) |> strtoupper(...);
var_dump($x);

$data = ["key" => "hello world"];
$data["key"] |>= str_replace(' ', '_', ?) |> strtoupper(...);
var_dump($data["key"]);

$x = "abc";
$x |>= substr(?, 1);
var_dump($x);

?>
--EXPECT--
string(9) "Hello PHP"
string(7) "trimmed"
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(4)
  [2]=>
  int(1)
}
string(10) "hello....."
string(11) "HELLO-WORLD"
string(11) "HELLO_WORLD"
string(2) "bc"
