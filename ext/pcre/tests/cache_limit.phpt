--TEST--
Compiled regex cache limit
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
define('PREG_CACHE_SIZE', 4096+1);

$re = '';
$str = str_repeat('x', PREG_CACHE_SIZE);

for ($i=0; $i < PREG_CACHE_SIZE; ++$i) {
	$re .= '.';
	if (!preg_match("/$re/", $str)) {
		die('non match. error');
	}
}

var_dump(preg_match('/./', $str));   // this one was already deleted from the cache
var_dump(preg_match("/$re/", $str)); // but not this one

echo "done\n";
?>
--EXPECT--
int(1)
int(1)
done
