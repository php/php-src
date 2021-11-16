--TEST--
FileInfo - Calling the constructor twice
--EXTENSIONS--
fileinfo
--FILE--
<?php

$x = new finfo;
$x->__construct();

echo "done!\n";

?>
--EXPECT--
done!
