--TEST--
Bug #33277 (private method accessed by child class)
--FILE--
<?php
class foo {
        private function bar() {
                echo "private!\n";
        }
}
 
class fooson extends foo {
        function barson() {
                $this->bar();
        }
}
 
class foo2son extends fooson {
 
        function bar() {
                echo "public!\n";
        }
}
 
$b = new foo2son();
$b->barson();
?>
--EXPECT--
public!
