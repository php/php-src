--TEST--
ReflectionClass::getProperty()
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
    try {
        echo "  --> Check for s: ";
        var_dump($rc->getProperty("s"));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        echo "  --> Check for a: ";
        var_dump($rc->getProperty("a"));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        echo "  --> Check for A: ";
        var_dump($rc->getProperty("A"));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        echo "  --> Check for doesNotExist: ";
        var_dump($rc->getProperty("doesNotExist"));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

}
?>
--EXPECTF--
Reflecting on class pubf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(4) "pubf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(4) "pubf"
}
  --> Check for A: ReflectionException: Property pubf::$A does not exist
  --> Check for doesNotExist: ReflectionException: Property pubf::$doesNotExist does not exist
Reflecting on class subpubf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(4) "pubf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(4) "pubf"
}
  --> Check for A: ReflectionException: Property subpubf::$A does not exist
  --> Check for doesNotExist: ReflectionException: Property subpubf::$doesNotExist does not exist
Reflecting on class protf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(5) "protf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(5) "protf"
}
  --> Check for A: ReflectionException: Property protf::$A does not exist
  --> Check for doesNotExist: ReflectionException: Property protf::$doesNotExist does not exist
Reflecting on class subprotf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(5) "protf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(5) "protf"
}
  --> Check for A: ReflectionException: Property subprotf::$A does not exist
  --> Check for doesNotExist: ReflectionException: Property subprotf::$doesNotExist does not exist
Reflecting on class privf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(5) "privf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(5) "privf"
}
  --> Check for A: ReflectionException: Property privf::$A does not exist
  --> Check for doesNotExist: ReflectionException: Property privf::$doesNotExist does not exist
Reflecting on class subprivf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(5) "privf"
}
  --> Check for a: ReflectionException: Property subprivf::$a does not exist
  --> Check for A: ReflectionException: Property subprivf::$A does not exist
  --> Check for doesNotExist: ReflectionException: Property subprivf::$doesNotExist does not exist
