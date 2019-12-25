--TEST--
#79029 (Use After Free's in XMLReader / XMLWriter)
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php
$x = array( new XMLWriter() );
$x[0]->openUri("bug79029.txt");
$x[0]->startComment();
@unlink("bug79029.txt");

$x = new XMLWriter();
$x->openUri("bug79029.txt");
fclose(@end(get_resources()));
@unlink("bug79029.txt");

file_put_contents("bug79029.txt", "a");
$x = new XMLReader();
$x->open("bug79029.txt");
fclose(@end(get_resources()));
@unlink("bug79029.txt");
?>
okey
--CLEAN--
<?php
@unlink("bug79029.txt");
?>
--EXPECTF--
Warning: fclose(): %d is not a valid stream resource in %sbug79029.php on line %d

Warning: fclose(): %d is not a valid stream resource in %sbug79029.php on line %d
okey
