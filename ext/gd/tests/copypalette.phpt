--TEST--
imagepalettecopy
--SKIPIF--
<?php
        if (!function_exists('imagecolorat')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$failed = false;
$im = imagecreate(1,1);
for ($i=0; $i<256; $i++) {
	imagecolorallocate($im, $i, $i, $i);
}

$im2 = imagecreate(1,1);
imagepalettecopy($im2, $im);

for ($i=0; $i<256; $i++) {
	$c = imagecolorsforindex($im2, $i);
	if ($c['red']!=$i || $c['green']!=$i || $c['blue']!=$i) {
		$failed = true;
		break;
	} 
}
echo "copy palette 255 colors: ";
echo $failed ? 'failed' : 'ok';
echo "\n";

$im = imagecreate(1,1);
$im2 = imagecreate(1,1);
imagecolorallocatealpha($im, 0,0,0,100);

imagepalettecopy($im2, $im);
$c = imagecolorsforindex($im2, 0);
if ($c['red']!=0 || $c['green']!=0 || $c['blue']!=0 || $c['alpha']!=100) {
	$failed = true;
} 
echo 'copy palette 1 color and alpha: ';
echo $failed ? 'failed' : 'ok';
echo "\n";
?>
--EXPECT--
copy palette 255 colors: ok
copy palette 1 color and alpha: ok
