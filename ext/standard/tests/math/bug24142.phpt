--TEST--
Bug #24142 (round() problems)
--FILE--
<?php // $Id$ vim600:syn=php
echo round(5.045, 2). "\n";
echo round(5.055, 2). "\n";
?>
--EXPECT--
5.05
5.06
