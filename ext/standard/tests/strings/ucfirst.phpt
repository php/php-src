--TEST--
ucfirst()
--FILE--
<?php
echo ucfirst("peren"), "\n";
echo ucfirst("appelen"), "\n";
echo ucfirst("ßen"), "\n";
?>
--EXPECT--
Peren
Appelen
SSen
