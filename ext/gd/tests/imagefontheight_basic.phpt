--TEST--
Testing imagefontheight() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--FILE--
<?php
var_dump(imagefontheight(1),imagefontheight(2),imagefontheight(3),imagefontheight(4),imagefontheight(5));
?>
--EXPECT--
int(8)
int(13)
int(13)
int(16)
int(15)
