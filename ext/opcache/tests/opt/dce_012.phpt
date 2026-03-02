--TEST--
Incorrect DCE of constructor DO_FCALL
--FILE--
<?php
function test() {
	$a = null;
    for ($i = 0; $i < 10; $i++) {
        $obj = $a = $a;
        $obj = new stdClass;
        $obj->orop1 = 'abc';
    }

    foreach (range(0, 6) as $levels) {
        print "$levels level" . ($levels == C ? "" : "s") . "aaa";
    }

    $obj->prop1 = null;
}
test();
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "C" in %sdce_012.php:11
Stack trace:
#0 %sdce_012.php(16): test()
#1 {main}
  thrown in %sdce_012.php on line 11
