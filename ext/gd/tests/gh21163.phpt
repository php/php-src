--TEST--
GH-17772 (prevents signed int overflow in gdImageCopy functions)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
?>
--FILE--
<?php
function makeImages(): array {
    $dst = imagecreatetruecolor(2, 2);
    $src = imagecreatetruecolor(2, 2);

    $blackDst = imagecolorallocate($dst, 0, 0, 0);
    $whiteSrc = imagecolorallocate($src, 255, 255, 255);

    imagefilledrectangle($dst, 0, 0, 1, 1, $blackDst);
    imagefilledrectangle($src, 0, 0, 1, 1, $whiteSrc);

    return [$dst, $src];
}

function assertDstUnchanged(string $label, $dst, int $expectedColor): void {
    $actual = imagecolorat($dst, 0, 0);
    if ($actual !== $expectedColor) {
        echo "FAIL $label: dst changed, got $actual expected $expectedColor\n";
        return;
    }
    echo "OK $label\n";
}

/*
  Use values that will overflow a 32 bit signed int when adding w or h.
  INT_MAX is 0x7fffffff.
*/
$nearIntMax = 0x7fffffff - 5;
$w = 10;
$h = 10;

/* imagecopy */
[$dst, $src] = makeImages();
$expected = imagecolorat($dst, 0, 0);
imagecopy($dst, $src, $nearIntMax, 0, 0, 0, $w, $h);
assertDstUnchanged('imagecopy', $dst, $expected);

/* imagecopymerge */
[$dst, $src] = makeImages();
$expected = imagecolorat($dst, 0, 0);
imagecopymerge($dst, $src, $nearIntMax, 0, 0, 0, $w, $h, 50);
assertDstUnchanged('imagecopymerge', $dst, $expected);

/* imagecopymergegray */
[$dst, $src] = makeImages();
$expected = imagecolorat($dst, 0, 0);
imagecopymergegray($dst, $src, $nearIntMax, 0, 0, 0, $w, $h, 50);
assertDstUnchanged('imagecopymergegray', $dst, $expected);

/* imagecopyresized */
[$dst, $src] = makeImages();
$expected = imagecolorat($dst, 0, 0);
imagecopyresized($dst, $src, $nearIntMax, 0, 0, 0, $w, $h, 1, 1);
assertDstUnchanged('imagecopyresized', $dst, $expected);

/* imagecopyresampled */
[$dst, $src] = makeImages();
$expected = imagecolorat($dst, 0, 0);
imagecopyresampled($dst, $src, $nearIntMax, 0, 0, 0, $w, $h, 1, 1);
assertDstUnchanged('imagecopyresampled', $dst, $expected);

echo "done\n";
?>
--EXPECT--
OK imagecopy
OK imagecopymerge
OK imagecopymergegray
OK imagecopyresized
OK imagecopyresampled
done
