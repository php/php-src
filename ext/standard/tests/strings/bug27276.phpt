--TEST--
Bug #27276 (str_replace tries to use obscene amounts of ram)
--FILE--
<?php
ini_set("memory_limit", "12m");
$replacement = str_repeat("x", 12444);
$string = str_repeat("x", 9432);
$key =    "{BLURPS}";

str_replace($key, $replacement, $string);

echo "Alive!\n";
?>
--EXPECT--
Alive!
