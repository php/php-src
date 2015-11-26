--TEST--
Check for JIT enablement status
--FILE--
<?php

ob_start();
phpinfo();
$info = ob_get_contents();
ob_end_clean();

var_dump(preg_match(",PCRE JIT Support .* enabled,", $info));
?>
--EXPECT--
int(1)
