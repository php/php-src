--TEST--
Test that the Directory extension constants are correctly defined.
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php

echo DIRECTORY_SEPARATOR;

echo "\n";

echo PATH_SEPARATOR;

echo "\n";

echo "done";

?>
--EXPECT--
/
:
done
