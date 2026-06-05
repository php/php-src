--TEST--
Importing functions whose aliases differ only by case is allowed (aliases are case-sensitive)
--FILE--
<?php

namespace {
    use function foo\bar;
    use function foo\BAR;
    echo "ok\n";
}

?>
--EXPECT--
ok
