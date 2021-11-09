--TEST--
Ability to do real offset from start of a zend_string for preg_match,
Previously unthinkable or documented as impossible, using front anchor ^ expression.
--FILE--
<?php
$s = "Hello world,Καλημέρα κόσμε,コンニチハ,Привет мир,你好 世界";
$exp = "/^(?:(\w+\s*\w*),?)/u";
$len = strlen($s);
$offset = 0;
while($offset < $len) {
	$match = null;
	$result = preg_match($exp, $s, $match,  PREG_START_LEN_OFFSET, $offset);
	if (empty($match)) {
		break;
	}
	$capture = $match[1];
	$offset += strlen($match[0]);
	echo $capture . PHP_EOL;
}
?>
--EXPECT--
Hello world
Καλημέρα κόσμε
コンニチハ
Привет мир
你好 世界
