--TEST--
Github #215 (imagefilltoborder stack overflow when invalid pallete index used)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
    die("skip test requires GD 2.2.2 or higher");
}
?>
--FILE--
<?php
$image = imagecreate( 10, 10 );
$bgd = imagecolorallocate( $image, 0, 0, 0 );
$border = imagecolorallocate( $image, 255, 0, 0 );
$fillcolor = imagecolorallocate( $image, 255, 0, 0 );

/* Use unallocated color index */
imagefilltoborder( $image, 0,0, $border+10, $fillcolor);
echo "#1 passes\n";

/* Use negative color index */
imagefilltoborder( $image, 0,0, -$border,  $fillcolor);
echo "#2 passes\n";


/* Use unallocated color index */
imagefilltoborder( $image, 0,0, $border, $fillcolor+10);
echo "#3 passes\n";

/* Use negative color index */
imagefilltoborder( $image, 0,0, $border, -$fillcolor);
echo "#4 passes\n";


/* Use negative color index */
imagefilltoborder( $image, 0,0, $border+10, $fillcolor+10);
echo "#5 passes";


?>
--EXPECT--
#1 passes
#2 passes
#3 passes
#4 passes
#5 passes
