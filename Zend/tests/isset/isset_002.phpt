--TEST--
Testing isset with several undefined variables as argument
--FILE--
<?php

var_dump(isset($a, ${$b}, $$c, $$$$d, $e[$f->g]->d));

?>
--EXPECT--
bool(false)
