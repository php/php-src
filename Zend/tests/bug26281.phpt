--TEST--
Bug #26281 (switch() crash when condition is a string offset)
--FILE--
<?php
    $x = 'abc';
    switch ($x[0]) {
        case 'a':
            echo "no crash\n";
            break;
    }
?>
--EXPECT--
no crash
