--TEST--
errmsg: disabled eval function
--INI--
disable_functions=eval
--FILE--
<?php

eval('echo "Eval";');

echo "Done\n";
?>
--EXPECTF--
Warning: eval() has been disabled for security reasons in %s on line %d
Done
