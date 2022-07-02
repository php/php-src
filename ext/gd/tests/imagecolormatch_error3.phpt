--TEST--
Send not TrueColor to Image 1 parameter imagecolormatch() of GD library
--CREDITS--
Paulo Alves de Sousa Filho <pspalves [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--FILE--
<?php
$ima = imagecreatetruecolor(110, 20);
$background_color = imagecolorallocate($ima, 0, 0, 0);
$imb = imagecreatetruecolor(110, 20);
$background_color = imagecolorallocate($imb, 0, 0, 100);

try {
    imagecolormatch($ima, $imb);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
imagecolormatch(): Argument #2 ($image2) must be Palette
