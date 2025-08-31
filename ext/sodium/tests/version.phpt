--TEST--
Check for libsodium version
--EXTENSIONS--
sodium
--FILE--
<?php
echo strlen(SODIUM_LIBRARY_VERSION) >= 5;
echo "\n";
echo SODIUM_LIBRARY_MAJOR_VERSION >= 4;
echo "\n";
echo SODIUM_LIBRARY_MINOR_VERSION >= 0;
?>
--EXPECT--
1
1
1
