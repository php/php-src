--TEST--
Edge-cases in elimination of JMPZ JMP with same target
--EXTENSIONS--
opcache
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
Warning: Undefined variable $undef in %s on line %d
done
