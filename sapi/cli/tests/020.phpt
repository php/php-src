--TEST--
CLI php --ri
--SKIPIF--
<?php
include "skipif.inc";
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die ("skip not for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');


echo `"$php" -n --ri this_extension_does_not_exist_568537753423`;
echo `"$php" -n --ri standard`;

echo "\nDone\n";
?>
--EXPECTF--
Extension 'this_extension_does_not_exist_568537753423' not present.

standard

%a

Directive => Local Value => Master Value
%a

Done
