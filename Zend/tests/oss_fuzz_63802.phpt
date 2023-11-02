--TEST--
oss-fuzz #63802: Post inc/dec with error leaks op1
--FILE--
<?php
class Foo {
    public function preInc() {
        ++$this > 42;
    }
    public function preDec() {
        --$this > 42;
    }
    public function postInc() {
        $this++ > 42;
    }
    public function postDec() {
        $this-- > 42;
    }
}
$foo = new Foo();
foreach (['pre', 'post'] as $prePost) {
    foreach (['inc', 'dec'] as $incDec) {
        try {
            $foo->{$prePost . ucfirst($incDec)}();
        } catch (TypeError $e) {
            echo $e->getMessage(), "\n";
        }
    }
}
?>
--EXPECT--
Cannot increment Foo
Cannot decrement Foo
Cannot increment Foo
Cannot decrement Foo
