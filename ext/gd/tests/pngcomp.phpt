--TEST--
png compression test
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available.");
	}

	if (!function_exists("imagecreatefrompng") || !function_exists("imagepng")) {
		die("skip png support unavailable");
	}
?>
--FILE--
<?php
	$cwd = __DIR__;

	echo "PNG compression test: ";

        $im = imagecreatetruecolor(20,20);
        imagefilledrectangle($im, 5,5, 10,10, 0xffffff);
        imagepng($im, $cwd . '/test_pngcomp.png', 9);

        $im2 = imagecreatefrompng($cwd . '/test_pngcomp.png');
        $col = imagecolorat($im2, 8,8);
        if ($col == 0xffffff) {
                echo "ok\n";
        }

	@unlink($cwd . "/test_pngcomp.png");
?>
--EXPECT--
PNG compression test: ok
