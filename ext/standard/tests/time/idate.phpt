--TEST--
idate() function
--FILE--
<?
$tmp = "UYzymndjHGhgistwLBIW";
for($a=0;$a<strlen($tmp);$a++){
	echo idate($tmp{$a},1043324459)."\n";
}
?>
--EXPECT--
1043324459
2003
22
3
1
1
23
23
12
12
12
12
20
59
31
4
0
556
0
4
