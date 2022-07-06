--TEST--
using different image sizes imagecolormatch() of GD library
--CREDITS--
Paulo Alves de Sousa Filho <pspalves [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$ima = imagecreatetruecolor(110, 20);
$background_color = imagecolorallocate($ima, 0, 0, 0);
$imb = imagecreate(100, 20);
$background_color = imagecolorallocate($imb, 0, 0, 100);

try {
    imagecolormatch($ima, $imb);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
imagecolormatch(): Argument #2 ($image2) must be the same size as argument #1 ($im1)
