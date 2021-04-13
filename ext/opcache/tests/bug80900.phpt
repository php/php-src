--TEST--
Bug 80900: Switch constant with incorrect type
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function switchLong() {
    $var = 'foo';
    /* The number of case clauses needs to be greater than 5,
     * otherwise it will not be compiled into SWITCH_LONG. */
    switch ($var) {
        case 1:
            echo 'no1';
            break;
        case 2:
            echo 'no2';
            break;
        case 3:
            echo 'no3';
            break;
        case 4:
            echo 'no4';
            break;
        case 5:
            echo 'no5';
            break;
        default:
            echo 'yes';
            break;
    }
    echo PHP_EOL;
}

function switchString() {
    $var = false;
    switch ($var) {
        case 'string':
            echo 'no';
            break;
        default:
            echo 'yes';
            break;
    }
    echo PHP_EOL;
}

switchLong();
switchString();
?>
--EXPECT--
yes
yes
