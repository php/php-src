--TEST--
Return type covariance in interface implementation
--FILE--
<?php
interface foo {
    public function bar() : foo;
}


class qux implements foo {
    public function bar() : qux {
        return $this;
    }
}

$qux = new qux();
echo get_class($qux->bar());

?>
--EXPECT--
qux
