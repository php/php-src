--TEST--
Pipe assign operator with mixed callable types in chains
--FILE--
<?php

function add_prefix(string $s): string { return "prefix_" . $s; }

class Transform {
    public static function upper(string $s): string { return strtoupper($s); }
}

$s = "hello";
$s |>= add_prefix(...) |> Transform::upper(...) |> (fn($s) => $s . "_done");
var_dump($s);

?>
--EXPECT--
string(17) "PREFIX_HELLO_done"
