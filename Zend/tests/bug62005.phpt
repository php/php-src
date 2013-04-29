--TEST--
Bug #62005 (unexpected behavior when incrementally assigning to a member of a null object)
--FILE--
<?php
function add_points($player, $points) {
    $player->energy += $points;
    print_r($player);
}
add_points(NULL, 2);
--EXPECTF--
Warning: Creating default object from empty value in %sbug62005.php on line %d

Notice: Undefined property: stdClass::$energy in %sbug62005.php on line 3
stdClass Object
(
    [energy] => 2
)
