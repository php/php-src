--TEST--
#79029 (Use After Free's in XMLReader / XMLWriter)
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php
$x = array( new XMLWriter() );
$x[0]->openUri("a");
$x[0]->startComment();
?>
okey
--EXPECT--
okey
