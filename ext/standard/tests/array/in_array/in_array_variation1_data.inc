<?php
enum Sample {
    case A;
    case B;
}

$values = [
    "integer 4" => 4,
    "string '4'" => "4",
    "float 4.00" => 4.00,
    "string 'b'" => "b",
    "string '5'" => "5",
    "integer -2" => -2,
    "float -2.0" => -2.0,
    "float -2.98989" => -2.98989,
    "string '-.9'" => "-.9",
    "string 'True'" => "True",
    "empty string" => "",
    "empty array" => [],
    "null" => NULL,
    "string 'ab'" => "ab",
    "string 'abcd'" => "abcd",
    "float 0.0" => 0.0,
    "integer -0" => -0,
    "string with null bytes" => "abcd\x00abcd\x00abcd",
    "enum Sample::A" => Sample::A,
    "enum Sample::B" => Sample::B,
];
