--TEST--
GH-17984: array of references handling
--EXTENSIONS--
gd
--FILE--
<?php
$a = 45;
$matrix = [&$a, 1, 3, 1, 5, 1];
$subarray = [&$a, 0, 0];
$matrix3 = array([0, 0, 0] , &$subarray, [0, 0, 0]);
$src = imagecreatetruecolor(8, 8);
var_dump(imageaffine($src, $matrix));
var_dump(imageaffinematrixconcat($matrix, $matrix));
var_dump(imageconvolution($src, $matrix3, 1.0, 0.0));
$poly = imagecolorallocate($src, 255, 0, 0);
var_dump(imagepolygon($src, array (
        &$a,   0,
        100, 200,
        300, 200
    ),
    $poly));
$style = [&$a, &$a];
var_dump(imagesetstyle($src, $style)); 
?>
--EXPECT--
object(GdImage)#2 (0) {
}
array(6) {
  [0]=>
  float(2028)
  [1]=>
  float(46)
  [2]=>
  float(138)
  [3]=>
  float(4)
  [4]=>
  float(233)
  [5]=>
  float(7)
}
bool(true)
bool(true)
bool(true)
