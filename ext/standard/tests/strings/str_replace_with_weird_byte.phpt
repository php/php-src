--TEST--
str_replace(): with various unprintable bytes
--INI--
precision=14
--FILE--
<?php

$subject = "I\thave a\0null\0byte! But also tab.";

/* needles in an array to be compared in the string $string */
$search_str = [
  "\0",
  "\t",
];

foreach ($search_str as $str) {
    var_dump(str_replace($str, "FOUND", $subject, $count) );
    var_dump($count);
}

?>
--EXPECTF--
string(41) "I	have aFOUNDnullFOUNDbyte! But also tab."
int(2)
string(37) "IFOUNDhave a%0null%0byte! But also tab."
int(1)
