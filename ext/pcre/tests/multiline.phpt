--TEST--
Multi-line match
--FILE--
<?php

var_dump(preg_match_all('/^.{2,3}$/', "aei\nou", $dummy));
var_dump(preg_match_all('/^.{2,3}$/', "aei\nou\n", $dummy));
var_dump(preg_match_all('/^.{2,3}$/m', "aei\nou", $dummy));
var_dump(preg_match_all('/^.{2,3}$/m', "aei\nou\n", $dummy));

echo "done\n";
?>
--EXPECT--
int(0)
int(0)
int(2)
int(2)
done
