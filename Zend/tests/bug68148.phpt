--TEST--
Bug #68148: $this is null inside include
--FILE--
<?php

class Test {
    public function method() {
        eval('var_dump($this);');
    }
}

(new Test)->method();

?>
--EXPECT--
object(Test)#1 (0) {
}
