--TEST--
#79029 (Use After Free's in XMLReader / XMLWriter)
--SKIPIF--
<?php 
if (!extension_loaded("xmlwriter")) print "skip xmlwriter extension not available";
if (!extension_loaded("xmlreader")) print "skip xmlreader extension not available";
?>
--FILE--
<?php
$x = array( new XMLWriter() );
$x[0]->openUri("bug79029_1.txt");
$x[0]->startComment();

$y = new XMLWriter();
$y->openUri("bug79029_2.txt");
fclose(@end(get_resources()));

file_put_contents("bug79029_3.txt", "a");
$z = new XMLReader();
$z->open("bug79029_3.txt");
fclose(@end(get_resources()));
?>
okey
--CLEAN--
<?php
@unlink("bug79029_1.txt");
@unlink("bug79029_2.txt");
@unlink("bug79029_3.txt");
?>
--EXPECTF--
Warning: fclose(): %d is not a valid stream resource in %sbug79029.php on line %d

Warning: fclose(): %d is not a valid stream resource in %sbug79029.php on line %d
okey
