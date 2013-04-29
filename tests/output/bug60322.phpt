--TEST--
Bug #60322 (ob_get_clean() now raises an E_NOTICE if no buffers exist)
--FILE--
<?php
while(ob_get_clean()) ;
var_dump(ob_get_clean());
--EXPECT--
bool(false)
