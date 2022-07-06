--TEST--
Bug #72688 (preg_match missing group names in matches)
--FILE--
<?php

$pattern = [];
for ($i = 0; $i < 300; $i++) {
    $pattern[] = "(?'group{$i}'{$i}$)";
}
$fullPattern = '/' . implode('|', $pattern) . '/uix';

preg_match($fullPattern, '290', $matches);

var_dump($matches['group290']);
?>
--EXPECT--
string(3) "290"
