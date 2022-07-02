--TEST--
Bug #55005 (imagepolygon num_points requirement)
--EXTENSIONS--
gd
--FILE--
<?php
require_once __DIR__ . '/func.inc';

$g = imagecreate(300, 300);
$bgnd = imagecolorallocate($g, 255, 255, 255);
$fgnd = imagecolorallocate($g, 0, 0, 0);
trycatch_dump(
    fn () => imagefilledpolygon($g, array(100,10, 100,100, 180,100), 2, $fgnd),
    fn () => imagepolygon($g, array(200,10, 200,100, 280,100), 2, $fgnd)
);
?>
--EXPECTF--
Deprecated: imagefilledpolygon(): Using the $num_points parameter is deprecated in %s on line %d
!! [ValueError] imagefilledpolygon(): Argument #3 ($num_points_or_color) must be greater than or equal to 3

Deprecated: imagepolygon(): Using the $num_points parameter is deprecated in %s on line %d
!! [ValueError] imagepolygon(): Argument #3 ($num_points_or_color) must be greater than or equal to 3
