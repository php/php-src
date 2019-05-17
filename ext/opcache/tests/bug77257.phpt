--TEST--
Bug #77257: value of variable assigned in a switch() construct gets lost
--FILE--
<?php
function test($x) {
    $a = false;
    switch($x["y"]) {
        case "a":
            $a = true;
            break;
        case "b":
            break;
        case "c":
            break;
    }
    return $a;
}
var_dump(test(["y" => "a"]));
?>
--EXPECT--
bool(true)
