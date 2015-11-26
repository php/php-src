--TEST--
Inherited constant from interface 
--FILE--
<?php
interface foo {
    const foo = 'foobar';
    public function bar($x = foo);
}

class foobar implements foo {
    const foo = 'bar';
    public function bar($x = foo::foo) {
        var_dump($x);
    }    
}
?>
--EXPECTF--
Fatal error: Cannot inherit previously-inherited or override constant foo from interface foo in %s on line %d
