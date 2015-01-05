--TEST--
Ensure __autoload() allows for recursive calls if the class name differs.
--FILE--
<?php
  function __autoload($name)
  {
      echo "IN:  " . __METHOD__ . "($name)\n";
      
      static $i = 0;
      if ($i++ > 10) {
          echo "-> Recursion detected - as expected.\n";
          return;
      }
      
      class_exists('UndefinedClass' . $i);
      
      echo "OUT: " . __METHOD__ . "($name)\n";
  }
  
  var_dump(class_exists('UndefinedClass0'));
?>
--EXPECTF--
IN:  __autoload(UndefinedClass0)
IN:  __autoload(UndefinedClass1)
IN:  __autoload(UndefinedClass2)
IN:  __autoload(UndefinedClass3)
IN:  __autoload(UndefinedClass4)
IN:  __autoload(UndefinedClass5)
IN:  __autoload(UndefinedClass6)
IN:  __autoload(UndefinedClass7)
IN:  __autoload(UndefinedClass8)
IN:  __autoload(UndefinedClass9)
IN:  __autoload(UndefinedClass10)
IN:  __autoload(UndefinedClass11)
-> Recursion detected - as expected.
OUT: __autoload(UndefinedClass10)
OUT: __autoload(UndefinedClass9)
OUT: __autoload(UndefinedClass8)
OUT: __autoload(UndefinedClass7)
OUT: __autoload(UndefinedClass6)
OUT: __autoload(UndefinedClass5)
OUT: __autoload(UndefinedClass4)
OUT: __autoload(UndefinedClass3)
OUT: __autoload(UndefinedClass2)
OUT: __autoload(UndefinedClass1)
OUT: __autoload(UndefinedClass0)
bool(false)

