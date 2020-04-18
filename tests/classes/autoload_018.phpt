--TEST--
Ensure __autoload() allows for recursive calls if the class name differs.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "IN:  autoload($name)\n";

  static $i = 0;
  if ($i++ > 10) {
      echo "-> Recursion detected - as expected.\n";
      return;
  }

  class_exists('UndefinedClass' . $i);

  echo "OUT: autoload($name)\n";
});

var_dump(class_exists('UndefinedClass0'));
?>
--EXPECT--
IN:  autoload(UndefinedClass0)
IN:  autoload(UndefinedClass1)
IN:  autoload(UndefinedClass2)
IN:  autoload(UndefinedClass3)
IN:  autoload(UndefinedClass4)
IN:  autoload(UndefinedClass5)
IN:  autoload(UndefinedClass6)
IN:  autoload(UndefinedClass7)
IN:  autoload(UndefinedClass8)
IN:  autoload(UndefinedClass9)
IN:  autoload(UndefinedClass10)
IN:  autoload(UndefinedClass11)
-> Recursion detected - as expected.
OUT: autoload(UndefinedClass10)
OUT: autoload(UndefinedClass9)
OUT: autoload(UndefinedClass8)
OUT: autoload(UndefinedClass7)
OUT: autoload(UndefinedClass6)
OUT: autoload(UndefinedClass5)
OUT: autoload(UndefinedClass4)
OUT: autoload(UndefinedClass3)
OUT: autoload(UndefinedClass2)
OUT: autoload(UndefinedClass1)
OUT: autoload(UndefinedClass0)
bool(false)
