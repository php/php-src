--TEST--
ReflectionClass::hasMethod()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class pubf {
    public function f() {}
    static public function s() {}
}
class subpubf extends pubf {
}

class protf {
    protected function f() {}
    static protected function s() {}
}
class subprotf extends protf {
}

class privf {
    private function f() {}
    static private function s() {}
}
class subprivf extends privf  {
}

$classes = array("pubf", "subpubf", "protf", "subprotf",
                 "privf", "subprivf");
foreach($classes as $class) {
    echo "Reflecting on class $class: \n";
    $rc = new ReflectionClass($class);
    echo "  --> Check for f(): ";
    var_dump($rc->hasMethod("f"));
    echo "  --> Check for s(): ";
    var_dump($rc->hasMethod("s"));
    echo "  --> Check for F(): ";
    var_dump($rc->hasMethod("F"));
    echo "  --> Check for doesNotExist(): ";
    var_dump($rc->hasMethod("doesNotExist"));
}
?>
--EXPECT--
Reflecting on class pubf: 
  --> Check for f(): bool(true)
  --> Check for s(): bool(true)
  --> Check for F(): bool(true)
  --> Check for doesNotExist(): bool(false)
Reflecting on class subpubf: 
  --> Check for f(): bool(true)
  --> Check for s(): bool(true)
  --> Check for F(): bool(true)
  --> Check for doesNotExist(): bool(false)
Reflecting on class protf: 
  --> Check for f(): bool(true)
  --> Check for s(): bool(true)
  --> Check for F(): bool(true)
  --> Check for doesNotExist(): bool(false)
Reflecting on class subprotf: 
  --> Check for f(): bool(true)
  --> Check for s(): bool(true)
  --> Check for F(): bool(true)
  --> Check for doesNotExist(): bool(false)
Reflecting on class privf: 
  --> Check for f(): bool(true)
  --> Check for s(): bool(true)
  --> Check for F(): bool(true)
  --> Check for doesNotExist(): bool(false)
Reflecting on class subprivf: 
  --> Check for f(): bool(true)
  --> Check for s(): bool(true)
  --> Check for F(): bool(true)
  --> Check for doesNotExist(): bool(false)
  
