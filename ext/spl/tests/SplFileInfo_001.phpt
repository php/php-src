--TEST--
Testing SplFileInfo calling the constructor twice
--FILE--
<?php
$x = new SplFileInfo(1);
$x->__construct(1);

echo "done!\n";
?>
--EXPECT--
done!
