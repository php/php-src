--TEST--
output buffering - stati
--INI--
opcache.optimization_level=0
--FILE--
<?php
$stati = array();
function oh($str, $flags) {
	global $stati;
	$stati[] = "$flags: $str";
	return $str;
}
ob_start("oh", 3);
echo "yes";
echo "!\n";
ob_flush();
echo "no";
ob_clean();
echo "yes!\n";
echo "no";
ob_end_clean();
print_r($stati);
?>
--EXPECT--
yes!
yes!
Array
(
    [0] => 1: yes
    [1] => 4: !

    [2] => 2: no
    [3] => 0: yes!

    [4] => 10: no
)
