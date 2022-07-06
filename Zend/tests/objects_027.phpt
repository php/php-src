--TEST--
Testing 'new static;' calling parent method
--FILE--
<?php

class bar {
    public function show() {
        var_dump(new static);
    }
}

class foo extends bar {
    public function test() {
        parent::show();
    }
}

$foo = new foo;
$foo->test();

call_user_func(array($foo, 'test'));

?>
--EXPECTF--
object(foo)#%d (0) {
}
object(foo)#%d (0) {
}
