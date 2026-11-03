--TEST--
GH-23674 (gd.png_ignore_warning suppresses libpng warnings)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!function_exists("imagecreatefrompng")) die("skip png support unavailable");
if (!GD_BUNDLED) die("skip requires bundled GD library");
?>
--FILE--
<?php
$file = __DIR__ . "/gh23674.png";
$data = file_get_contents($file);

echo "gd.png_ignore_warning=0\n";
var_dump(imagecreatefrompng($file) instanceof GdImage);
var_dump(imagecreatefromstring($data) instanceof GdImage);

ini_set("gd.png_ignore_warning", "1");
echo "gd.png_ignore_warning=1\n";
var_dump(imagecreatefrompng($file) instanceof GdImage);
var_dump(imagecreatefromstring($data) instanceof GdImage);
var_dump(imagecreatefrompng("data://image/png;base64," . base64_encode($data)) instanceof GdImage);

echo "fatal errors are still reported\n";
var_dump(imagecreatefromstring(substr($data, 0, 60)));
?>
--EXPECTF--
gd.png_ignore_warning=0

Warning: imagecreatefrompng(): gd-png: libpng warning: iCCP: %s in %s on line %d
bool(true)

Warning: imagecreatefromstring(): gd-png: libpng warning: iCCP: %s in %s on line %d
bool(true)
gd.png_ignore_warning=1
bool(true)
bool(true)
bool(true)
fatal errors are still reported

Warning: imagecreatefromstring(): gd-png:  fatal libpng error: %s
%A
bool(false)
