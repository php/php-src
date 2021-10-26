--TEST--
Bug #71316 (libpng warning from imagecreatefromstring)
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip gd extension not available");
if (!(imagetypes() & IMG_PNG)) die("skip PNG support not available");
if (!GD_BUNDLED && version_compare(GD_VERSION, "2.3.3", '<=')) {
    die("skip test requires GD > 2.3.3");
}
?>
--FILE--
<?php
var_dump(
    @imagecreatefrompng(__DIR__ . '/bug71316.png')
);
?>
--EXPECTF--
resource(%d) of type (gd)
