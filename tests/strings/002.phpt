--TEST--
Formatted print functions
--FILE--
<?php 
error_reporting(0);

$fp = fopen("php://stdout", "w") or die("Arrggsgg!!");
$x = fprintf($fp, "fprintf test 1:%.5s", "abcdefghij");
echo "\n";
var_dump($x);

printf("printf test 1:%s\n", "simple string");
printf("printf test 2:%d\n", 42);
printf("printf test 3:%f\n", 10.0/3);
printf("printf test 4:%.10f\n", 10.0/3);
printf("printf test 5:%-10.2f\n", 2.5);
printf("printf test 6:%-010.2f\n", 2.5);
printf("printf test 7:%010.2f\n", 2.5);
printf("printf test 8:<%20s>\n", "foo");
printf("printf test 9:<%-20s>\n", "bar");
printf("printf test 10: 123456789012345\n");
printf("printf test 10:<%15s>\n", "høyesterettsjustitiarius");
printf("printf test 11: 123456789012345678901234567890\n");
printf("printf test 11:<%30s>\n", "høyesterettsjustitiarius");
printf("printf test 12:%5.2f\n", -12.34);
printf("printf test 13:%5d\n", -12);
printf("printf test 14:%c\n", 64);
printf("printf test 15:%b\n", 170);
printf("printf test 16:%x\n", 170);
printf("printf test 17:%X\n", 170);
printf("printf test 18:%16b\n", 170);
printf("printf test 19:%16x\n", 170);
printf("printf test 20:%16X\n", 170);
printf("printf test 21:%016b\n", 170);
printf("printf test 22:%016x\n", 170);
printf("printf test 23:%016X\n", 170);
printf("printf test 24:%.5s\n", "abcdefghij");
printf("printf test 25:%-2s\n", "gazonk");
printf("printf test 26:%2\$d %1\$d\n", 1, 2);
printf("printf test 27:%3\$d %d %d\n", 1, 2, 3);
printf("printf test 28:%2\$02d %1\$2d\n", 1, 2);
printf("printf test 29:%2\$-2d %1\$2d\n", 1, 2);
print("printf test 30:"); printf("%0\$s", 1); print("x\n");
vprintf("vprintf test 1:%2\$-2d %1\$2d\n", array(1, 2));


?>
--EXPECT--
fprintf test 1:abcde
int(20)
printf test 1:simple string
printf test 2:42
printf test 3:3.333333
printf test 4:3.3333333333
printf test 5:2.50      
printf test 6:2.50000000
printf test 7:0000002.50
printf test 8:<                 foo>
printf test 9:<bar                 >
printf test 10: 123456789012345
printf test 10:<høyesterettsjustitiarius>
printf test 11: 123456789012345678901234567890
printf test 11:<      høyesterettsjustitiarius>
printf test 12:-12.34
printf test 13:  -12
printf test 14:@
printf test 15:10101010
printf test 16:aa
printf test 17:AA
printf test 18:        10101010
printf test 19:              aa
printf test 20:              AA
printf test 21:0000000010101010
printf test 22:00000000000000aa
printf test 23:00000000000000AA
printf test 24:abcde
printf test 25:gazonk
printf test 26:2 1
printf test 27:3 1 2
printf test 28:02  1
printf test 29:2   1
printf test 30:x
vprintf test 1:2   1
