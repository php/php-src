--TEST--
errmsg: cannot inherit previously inherited constant
--FILE--
<?php

interface test1 {
    const FOO = 10;
}

interface test2 {
    const FOO = 10;
}

class test implements test1, test2 {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Class test inherits both test1::FOO and test2::FOO, which is ambiguous in %s on line %d
