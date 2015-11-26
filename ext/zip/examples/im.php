<?php
/* $Id$ */
$im = imagecreatefromgif('zip://' . dirname(__FILE__) . '/test_im.zip#pear_item.gif');
imagepng($im, 'a.png');

$z = new ZipArchive();
$z->open(dirname(__FILE__) . '/test_im.zip');
$im_string = $z->getFromName("pear_item.gif");
$im = imagecreatefromstring($im_string);
imagepng($im, 'b.png');

