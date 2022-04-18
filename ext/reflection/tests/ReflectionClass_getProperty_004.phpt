--TEST--
ReflectionClass::getProperty()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class A {
    public $pubC = "pubC in A";
    protected $protC = "protC in A";
    private $privC = "privC in A";

    public $pubA = "pubA in A";
    protected $protA = "protA in A";
    private $privA = "privA in A";
}

class B extends A {
    public $pubC = "pubC in B";
    protected $protC = "protC in B";
    private $privC = "privC in B";

    public $pubB = "pubB in B";
    protected $protB = "protB in B";
    private $privB = "privB in B";
}

class C extends B {
    public $pubC = "pubC in C";
    protected $protC = "protC in C";
    private $privC = "privC in C";
}

class X {
    public $pubC = "pubC in X";
    protected $protC = "protC in X";
    private $privC = "privC in X";
}

$myC = new C;
$rc = new ReflectionClass("C");

function showInfo($name) {
    global $rc, $myC;
    echo "--- (Reflecting on $name) ---\n";
    try {
        $rp = $rc->getProperty($name);
    } catch (Exception $e) {
        echo $e->getMessage() . "\n";
        return;
    }
    try {
        var_dump($rp);
        var_dump($rp->getValue($myC));
    } catch (Exception $e) {
        echo $e->getMessage() . "\n";
        return;
    }
}


showInfo("pubA");
showInfo("protA");
showInfo("privA");

showInfo("pubB");
showInfo("protB");
showInfo("privB");

showInfo("pubC");
showInfo("protC");
showInfo("privC");
showInfo("doesNotExist");

showInfo("A::pubC");
showInfo("A::protC");
showInfo("A::privC");

showInfo("B::pubC");
showInfo("B::protC");
showInfo("B::privC");

showInfo("c::pubC");
showInfo("c::PUBC");
showInfo("C::pubC");
showInfo("C::protC");
showInfo("C::privC");

showInfo("X::pubC");
showInfo("X::protC");
showInfo("X::privC");
showInfo("X::doesNotExist");

showInfo("doesNotexist::doesNotExist");

?>
--EXPECTF--
--- (Reflecting on pubA) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(4) "pubA"
  ["class"]=>
  string(1) "A"
}
string(9) "pubA in A"
--- (Reflecting on protA) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "protA"
  ["class"]=>
  string(1) "A"
}
string(10) "protA in A"
--- (Reflecting on privA) ---
Property C::$privA does not exist
--- (Reflecting on pubB) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(4) "pubB"
  ["class"]=>
  string(1) "B"
}
string(9) "pubB in B"
--- (Reflecting on protB) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "protB"
  ["class"]=>
  string(1) "B"
}
string(10) "protB in B"
--- (Reflecting on privB) ---
Property C::$privB does not exist
--- (Reflecting on pubC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(4) "pubC"
  ["class"]=>
  string(1) "C"
}
string(9) "pubC in C"
--- (Reflecting on protC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "protC"
  ["class"]=>
  string(1) "C"
}
string(10) "protC in C"
--- (Reflecting on privC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "privC"
  ["class"]=>
  string(1) "C"
}
string(10) "privC in C"
--- (Reflecting on doesNotExist) ---
Property C::$doesNotExist does not exist
--- (Reflecting on A::pubC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(4) "pubC"
  ["class"]=>
  string(1) "A"
}
string(9) "pubC in C"
--- (Reflecting on A::protC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "protC"
  ["class"]=>
  string(1) "A"
}
string(10) "protC in C"
--- (Reflecting on A::privC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "privC"
  ["class"]=>
  string(1) "A"
}
string(10) "privC in A"
--- (Reflecting on B::pubC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(4) "pubC"
  ["class"]=>
  string(1) "B"
}
string(9) "pubC in C"
--- (Reflecting on B::protC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "protC"
  ["class"]=>
  string(1) "B"
}
string(10) "protC in C"
--- (Reflecting on B::privC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "privC"
  ["class"]=>
  string(1) "B"
}
string(10) "privC in B"
--- (Reflecting on c::pubC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(4) "pubC"
  ["class"]=>
  string(1) "C"
}
string(9) "pubC in C"
--- (Reflecting on c::PUBC) ---
Property C::$PUBC does not exist
--- (Reflecting on C::pubC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(4) "pubC"
  ["class"]=>
  string(1) "C"
}
string(9) "pubC in C"
--- (Reflecting on C::protC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "protC"
  ["class"]=>
  string(1) "C"
}
string(10) "protC in C"
--- (Reflecting on C::privC) ---
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(5) "privC"
  ["class"]=>
  string(1) "C"
}
string(10) "privC in C"
--- (Reflecting on X::pubC) ---
Fully qualified property name X::$pubC does not specify a base class of C
--- (Reflecting on X::protC) ---
Fully qualified property name X::$protC does not specify a base class of C
--- (Reflecting on X::privC) ---
Fully qualified property name X::$privC does not specify a base class of C
--- (Reflecting on X::doesNotExist) ---
Fully qualified property name X::$doesNotExist does not specify a base class of C
--- (Reflecting on doesNotexist::doesNotExist) ---
Class "doesnotexist" does not exist
