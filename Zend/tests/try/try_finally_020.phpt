--TEST--
Combination of foreach, finally and exception (call order)
--FILE--
<?php
class A {
	public $n = 0;
	function __construct($n) {
		$this->n = $n;
	}
	function __destruct() {
		echo "destruct" . $this->n . "\n";
	}
}

foreach ([new A(1)] as $a) {
    $a = null;
    try {
        foreach ([new A(2)] as $a) {
            $a = null;
            try {
                foreach ([new A(3)] as $a) {
                    $a = null;
                    throw new Exception();
                }
            } finally {
                echo "finally1\n";
            }
        }
    } catch (Exception $e) {
        echo "catch\n";
    } finally {
        echo "finally2\n";
    }
}
?>
--EXPECT--
destruct3
finally1
destruct2
catch
finally2
destruct1
