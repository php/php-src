--TEST--
chr() and ord() functions
--POST--
--GET--
--FILE--
<?php
echo "Testing ord() & chr()...";
for($i=0; $i<256; $i++) echo !ord(chr($i)) == $i;
echo " done";
?>
--EXPECT--
Testing ord() & chr()... done
