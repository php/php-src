--TEST--
Bug #24594 (Filling an area using tiles).
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available\n");
	}
?>
--FILE--
<?php
	$tile = imagecreate(36,36);
	$base = imagecreate(150,150);
	$white = imagecolorallocate($tile,255,255,255);
	$black = imagecolorallocate($tile,0,0,0);
	$white = imagecolorallocate($base,255,255,255);
	$black = imagecolorallocate($base,0,0,0);

	/* create the dots pattern */
	for ($x=0;$x<36;$x+=2) {
		for ($y=0;$y<36;$y+=2) {
			imagesetpixel($tile,$x,$y,$black);
		}
	}

	imagesettile($base,$tile);
	imagerectangle($base, 9,9,139,139,$black);
	imageline($base, 9,9,139,139,$black);
	imagefill($base,11,12,IMG_COLOR_TILED);

	$res = imagecolorat($base,0,10)==$black?'1':'0';
	$res .= imagecolorat($base,0,20)==$black?'1':'0';
	$res .= imagecolorat($base,0,30)==$black?'1':'0';
	$res .= imagecolorat($base,0,40)==$black?'1':'0';
	$res .= imagecolorat($base,0,50)==$black?'1':'0';
	$res .= imagecolorat($base,0,60)==$black?'1':'0';

	$res .= imagecolorat($base,11,12)==$white?'1':'0';
	$res .= imagecolorat($base,12,13)==$white?'1':'0';
	$res .= imagecolorat($base,13,14)==$white?'1':'0';
	$res .= imagecolorat($base,14,15)==$white?'1':'0';
	$res .= imagecolorat($base,15,16)==$white?'1':'0';
	$res .= imagecolorat($base,16,17)==$white?'1':'0';

	$res .= imagecolorat($base,10,12)==$black?'1':'0';
	$res .= imagecolorat($base,11,13)==$black?'1':'0';
	$res .= imagecolorat($base,12,14)==$black?'1':'0';
	$res .= imagecolorat($base,13,15)==$black?'1':'0';
	$res .= imagecolorat($base,14,16)==$black?'1':'0';
	$res .= imagecolorat($base,15,17)==$black?'1':'0';
	echo "$res\n";

	imagefilledrectangle($base,0,0,149,149,$white);
	imagerectangle($base, 9,9,139,139,$black);
	imageline($base, 9,9,139,139,$black);
	imagefill($base,0,0,IMG_COLOR_TILED);

	$res = imagecolorat($base,0,10)==$black?'1':'0';
	$res .= imagecolorat($base,0,20)==$black?'1':'0';
	$res .= imagecolorat($base,0,30)==$black?'1':'0';
	$res .= imagecolorat($base,0,40)==$black?'1':'0';
	$res .= imagecolorat($base,0,50)==$black?'1':'0';
	$res .= imagecolorat($base,0,60)==$black?'1':'0';

	$res .= imagecolorat($base,11,12)==$white?'1':'0';
	$res .= imagecolorat($base,12,13)==$white?'1':'0';
	$res .= imagecolorat($base,13,14)==$white?'1':'0';
	$res .= imagecolorat($base,14,15)==$white?'1':'0';
	$res .= imagecolorat($base,15,16)==$white?'1':'0';
	$res .= imagecolorat($base,16,17)==$white?'1':'0';

	$res .= imagecolorat($base,0,16)==$black?'1':'0';
	$res .= imagecolorat($base,2,42)==$black?'1':'0';
	$res .= imagecolorat($base,4,44)==$black?'1':'0';
	$res .= imagecolorat($base,146,146)==$black?'1':'0';
	$res .= imagecolorat($base,148,146)==$black?'1':'0';
	$res .= imagecolorat($base,0,0)==$black?'1':'0';

	echo "$res\n";
?>
--EXPECT--
000000111111101010
111111111111111111
