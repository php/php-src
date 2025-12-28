--TEST--
Pipe binds higher than coalesce
--FILE--
<?php

function get_username(int $a): string {
    return (string)$a;
}

$user = 5
     |> get_username(...)
     ?? 'default';

var_dump($user);
?>
--EXPECT--
string(1) "5"
