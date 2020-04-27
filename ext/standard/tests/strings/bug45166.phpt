--TEST--
Bug #45166 (substr() )
--FILE--
<?php
    echo substr('cd', -3) . "\n";
?>
--EXPECTF--
Notice: substr(): Argument #2 ($start) is not contained in argument #1 ($str) in %s on line %d
cd
