--TEST--
Bug #52879 (Objects unreferenced in __get, __set, __isset or __unset can be freed too early)
--FILE--
<?php
class MyClass {
    public $myRef;
    public function __set($property,$value) {
        $this->myRef = $value;
    }
}
$myGlobal=new MyClass();
$myGlobal->myRef=&$myGlobal;
$myGlobal->myNonExistentProperty="ok\n";
echo $myGlobal;
?>
--EXPECT--
ok
