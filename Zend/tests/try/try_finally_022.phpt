--TEST--
Try finally (exception in "return" statement)
--FILE--
<?php
class A {
    public $x = 1;
    public $y = 2;
    function __destruct() {
        throw new Exception();
    }
}
try{
    $a = 0;
    switch ($a) {
        case 0:
    }
    switch ($a) {
        case 0:
    }
    switch ($a) {
        case 0:
    }
    foreach([new stdClass()] as $x) {
        foreach(new A() as $a) {
            foreach([new stdClass()] as $y) {
                try {
                    if (0) { echo "0" . (int)5; }
                    return $a;
                } catch (Exception $e) {
                    echo "exception1\n";
                }
            }
        }
    }
} catch (Exception $e) {
    echo "exception2\n";
}
?>
--EXPECT--
exception2
