--TEST--
Bug #26973 (*printf() '+' modifier problem)
--FILE--
<?php

printf("%+05d\n", 200);
printf("%+05d\n", -200);
printf("%+05f\n", 200);
printf("%+05f\n", -200);
printf("%+05u\n", 200);
printf("%+05u\n", -200);

?>
--EXPECT--
+0200
-0200
+0200.000000
-0200.000000
00200
4294967096
