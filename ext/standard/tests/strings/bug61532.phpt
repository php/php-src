--TEST--
printf custom padding modiffier bug #61532
--FILE--
<?php
printf("%'.9s\n","foo");
printf("%'.09s\n","foo");
?>
--EXPECT--
......foo
......foo
