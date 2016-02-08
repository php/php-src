--TEST--
Bug #65785 (mbstring.func_overload should be depreciated)
--INI--
mbstring.func_overload=2
--FILE--
<?php
echo ini_get('mbstring.func_overload');?>
--EXPECTF--
Deprecated: Unknown: Use of mbstring.func_overload is deprecated in Unknown on line 0
2
