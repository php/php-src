--TEST--
imageistruecolor, truecolortopalette
--SKIPIF--
<?php
        if (!function_exists('imagetruecolortopalette')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$im = imagecreatetruecolor(1,1); 
if (imageistruecolor($im)) echo "ok\n";

if (imagetruecolortopalette($im, 1,2)) echo "ok\n";
if (!imageistruecolor($im)) echo "ok\n";

?>
--EXPECTF--
ok
ok
ok
