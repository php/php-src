--TEST--
Edge-cases in elimination of JMPZ JMP with same target
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$foo = "foo";
if ($foo . "bar") { goto label; }
label:
if ($undef) { goto label2; }
label2:
echo "done\n";

?>
--EXPECTF--
Notice: Undefined variable: undef in %s on line %d
done
