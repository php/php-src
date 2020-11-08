--TEST--
imageline no AA
--SKIPIF--
<?php
        if (!function_exists('imageline')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);

// Horizontal line
imageline($im, 0,5, 5,5, 0x00ff00);

$p1 = imagecolorat($im, 0,5)==0x00ff00;
$p2 = imagecolorat($im, 5,5)==0x00ff00;
$p3 = true;
for ($x=1; $x<5; $x++) {
    $p3 = $p3 && (imagecolorat($im, $x,5)==0x00ff00);
}
if ($p1 && $p2 && $p3) {
    echo "Horizontal: ok\n";
}

$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);

imageline($im, 0,0, 0,5, 0x00ff00);
$p1 = imagecolorat($im, 0,0)==0x00ff00;
$p2 = imagecolorat($im, 0,5)==0x00ff00;
$p3 = true;
for ($y=1; $y<5; $y++) {
    $p3 = $p3 && (imagecolorat($im, 0,$y)==0x00ff00);
}

if ($p1 && $p2 && $p3) {
    echo "Vertical: ok\n";
}


$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);
imageline($im, 0,0, 5,5, 0x00ff00);


// Diagonal
$p1 = imagecolorat($im, 0,0)==0x00ff00;
$p2 = imagecolorat($im, 5,5)==0x00ff00;
$x=1;
$p3 = true;

for ($y=1; $y<5; $y++) {
    $p3 = $p3 && (imagecolorat($im, $x,$y)==0x00ff00);
    $x++;
}

if ($p1 && $p2 && $p3) {
    echo "Diagonal: ok\n";
}

// Outside
$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);
imageline($im, 12, 12, 23,23, 0x00ff00);
$p3 = true;
for ($x=0; $x<6; $x++) {
    for ($y=0; $y<6; $y++) {
        $p3 = $p3 && (imagecolorat($im, $x,$y)!=0x00ff00);
    }
}
if ($p3) {
    echo "Outside 1: ok\n";
}

$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);
imageline($im, -12, -12, -23,-23, 0x00ff00);
$p3 = true;
for ($x=0; $x<6; $x++) {
    for ($y=0; $y<6; $y++) {
        $p3 = $p3 && (imagecolorat($im, $x,$y)!=0x00ff00);
    }
}
if ($p3) {
    echo "Outside 2: ok\n";
}

$im = imagecreatetruecolor(6,6);
imagefill($im, 0,0, 0xffffff);
imageline($im, -1, -1, 4,4, 0x00ff00);
$p3 = true;
for ($x=0; $x<5; $x++) {
    for ($y=0; $y<5; $y++) {
        $p3 = $p3 && (imagecolorat($im, $x,$y)==0x00ff00);
    }
}
if ($p3) {
    echo "Outside 2: ok\n";
}


?>
--EXPECT--
Horizontal: ok
Vertical: ok
Diagonal: ok
Outside 1: ok
Outside 2: ok
