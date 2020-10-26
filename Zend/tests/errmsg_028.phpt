--TEST--
errmsg: cannot use 'self' as class name
--FILE--
<?php

class self {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Class "self" cannot be declared, self is a reserved class name in %s on line %d
