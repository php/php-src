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
Warning: Attempt to assign property 'energy' of non-object in %s on line %d
