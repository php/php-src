--TEST--
Bug #29538 (number_format and problem with 0)
--FILE--
<?php
    echo number_format(0.25, 2, '', ''), "\n";
    echo number_format(1234, 2, '', ',');
?>
--EXPECT--
025
1,23400
