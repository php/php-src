--TEST--
Bug #78346 (strip_tags no longer handling nested php tags)
--FILE--
<?php
$str = '<?= \'<?= 1 ?>\' ?>2';
var_dump(strip_tags($str));
?>
--EXPECT--
string(1) "2"
