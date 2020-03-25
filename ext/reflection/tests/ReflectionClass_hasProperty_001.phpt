--TEST--
ReflectionClass::hasProperty()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class pubf {
    public $a;
    static public $s;
}
class subpubf extends pubf {
}

class protf {
    protected $a;
    static protected $s;
}
class subprotf extends protf {
}

class privf {
    private $a;
    static protected $s;
}
class subprivf extends privf  {
}

$classes = array("pubf", "subpubf", "protf", "subprotf",
                 "privf", "subprivf");
foreach($classes as $class) {
    echo "Reflecting on class $class: \n";
    $rc = new ReflectionClass($class);
    echo "  --> Check for s: ";
    var_dump($rc->hasProperty("s"));
    echo "  --> Check for a: ";
    var_dump($rc->hasProperty("a"));
    echo "  --> Check for A: ";
    var_dump($rc->hasProperty("A"));
    echo "  --> Check for doesNotExist: ";
    var_dump($rc->hasProperty("doesNotExist"));
}
?>
--EXPECT--
Reflecting on class pubf: 
  --> Check for s: bool(true)
  --> Check for a: bool(true)
  --> Check for A: bool(false)
  --> Check for doesNotExist: bool(false)
Reflecting on class subpubf: 
  --> Check for s: bool(true)
  --> Check for a: bool(true)
  --> Check for A: bool(false)
  --> Check for doesNotExist: bool(false)
Reflecting on class protf: 
  --> Check for s: bool(true)
  --> Check for a: bool(true)
  --> Check for A: bool(false)
  --> Check for doesNotExist: bool(false)
Reflecting on class subprotf: 
  --> Check for s: bool(true)
  --> Check for a: bool(true)
  --> Check for A: bool(false)
  --> Check for doesNotExist: bool(false)
Reflecting on class privf: 
  --> Check for s: bool(true)
  --> Check for a: bool(true)
  --> Check for A: bool(false)
  --> Check for doesNotExist: bool(false)
Reflecting on class subprivf: 
  --> Check for s: bool(true)
  --> Check for a: bool(false)
  --> Check for A: bool(false)
  --> Check for doesNotExist: bool(false)
