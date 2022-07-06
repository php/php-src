--TEST--
Type inference 001: Invalid type narrowing warning
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    $j = 0;
    for ($i = 0; $i = 10; $i++) {
        $e;
        $a = $a;
        $obj->$e;
        $i += $a;
        $j++;
        $e;
        $a == $Z + $a .= $i+= $a;
        $j++;
        $e;
        $a == $Z + $j++;
        $e;
        $a == $Z + $a = $a + $b = $i += $a;
        $obj = new stdClass;
        $obj->prop1 = set_error_handler(function () {
            $$GLOBALS['a'] = null;
        });
        $obj->$a .= $i += $a;
        $obj = new stdClass;
        $obj->prop1 = $j++;
        $e;
        $a == $Z + $a = $a + $j++;
        $e;
        $a == $Z + $a = $a + $b = $aa = $a;
    }
}
test();
?>
DONE
--EXPECTF--
Warning: Undefined variable $a in %sinference_001.php on line 6

Warning: Undefined variable $obj in %sinference_001.php on line 7

Warning: Undefined variable $e in %sinference_001.php on line 7

Warning: Attempt to read property "" on null in %sinference_001.php on line 7

Warning: Undefined variable $Z in %sinference_001.php on line 11

Warning: Undefined variable $Z in %sinference_001.php on line 14

Warning: Undefined variable $Z in %sinference_001.php on line 16

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Warning: Array to string conversion in %sinference_001.php on line 19

Fatal error: Uncaught TypeError: Unsupported operand types: null + string in %sinference_001.php:11
Stack trace:
#0 %sinference_001.php(30): test()
#1 {main}
  thrown in %sinference_001.php on line 11
