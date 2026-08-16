--TEST--
Bug #28800 (Incorrect string to number conversion for strings starting with 'inf')
--FILE--
<?php
    $strings = array('into', 'info', 'inf', 'infinity', 'infin', 'inflammable');
    foreach ($strings as $v) {
        try {
            echo ($v+0)."\n";
        } catch (\TypeError $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
?>
--EXPECT--
TypeError: Unsupported operand types: string + int
TypeError: Unsupported operand types: string + int
TypeError: Unsupported operand types: string + int
TypeError: Unsupported operand types: string + int
TypeError: Unsupported operand types: string + int
TypeError: Unsupported operand types: string + int
