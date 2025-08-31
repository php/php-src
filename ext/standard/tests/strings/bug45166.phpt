--TEST--
Bug #45166 (substr() )
--FILE--
<?php
    echo substr('cd', -3) . "\n";
?>
--EXPECT--
cd
