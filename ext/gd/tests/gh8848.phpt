--TEST--
GH-8848 (imagecopyresized() error refers to the wrong argument)
--EXTENSIONS--
gd
--FILE--
<?php
$image1 = imagecreatetruecolor(1, 1);
$image2 = imagecreatetruecolor(1, 1);

$argslist = [
    [0, 1, 1, 1],
    [1, 0, 1, 1],
    [1, 1, 0, 1],
    [1, 1, 1, 0],
];

foreach ($argslist as $args) {
    try {
        imagecopyresized($image1, $image2, 1, 1, 1, 1, ...$args);
    } catch (ValueError $ex) {
        echo $ex->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
imagecopyresized(): Argument #7 ($dst_width) must be greater than 0
imagecopyresized(): Argument #8 ($dst_height) must be greater than 0
imagecopyresized(): Argument #9 ($src_width) must be greater than 0
imagecopyresized(): Argument #10 ($src_height) must be greater than 0
