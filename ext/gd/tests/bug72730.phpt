--TEST--
Bug #72730: imagegammacorrect allows arbitrary write access
--EXTENSIONS--
gd
--FILE--
<?php

require __DIR__ . '/func.inc';

$img =  imagecreatetruecolor(1, 1);

trycatch_dump(
    fn() => imagegammacorrect($img, -1, 1337)
);

?>
--EXPECT--
!! [ValueError] imagegammacorrect(): Argument #2 ($input_gamma) must be greater than 0
