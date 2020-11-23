--TEST--
Bug #49893 (Crash while creating an instance of Zend_Mail_Storage_Pop3)
--FILE--
<?php
class A {
    function __destruct() {
        try {
            throw new Exception("2");
        } catch (Exception $e) {
            echo $e->getMessage() . "\n";
        }
    }
}
class B {
    function __construct() {
        $this->a = new A();
        throw new Exception("1");
    }
}
try {
    $b = new B();
} catch(Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
2
1
