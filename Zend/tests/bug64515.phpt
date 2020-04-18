--TEST--
Bug #64515 (Memoryleak when using the same variablename 2times in function declaration) (PHP7)
--FILE--
<?php
function foo($unused = null, $unused = null, $arg = array()) {
        return 1;
}
foo();
echo "okey";
?>
--EXPECTF--
Fatal error: Redefinition of parameter $unused in %sbug64515.php on line 2
