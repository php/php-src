--TEST--
Bug #72696 (imagefilltoborder stackoverflow on truecolor images)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(10, 10);
imagefilltoborder($im, 0, 0, 1, -2);
?>
===DONE===
--EXPECT--
===DONE===
