--TEST--
ReflectionMethod class getFileName(), getStartLine() and getEndLine() methods
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

function reflectMethod($class, $method) {
    $methodInfo = new ReflectionMethod($class, $method);
    echo "**********************************\n";
    echo "Reflecting on method $class::$method()\n\n";
    echo "\ngetFileName():\n";
    var_dump($methodInfo->getFileName());
    echo "\ngetStartLine():\n";
    var_dump($methodInfo->getStartLine());
    echo "\ngetEndLine():\n";
    var_dump($methodInfo->getEndLine());
    echo "\n**********************************\n";
}

class TestClass
{
    public function foo() {


        echo "Called foo()\n";


    }

    static function stat() {
        echo "Called stat()\n";
    }

    private function priv() {
        echo "Called priv()\n";
    }

    protected function prot() {}

    public function __destruct() {}
}

class DerivedClass extends TestClass {}

interface TestInterface {
    public function int();
}

reflectMethod("DerivedClass", "foo");
reflectMethod("TestClass", "stat");
reflectMethod("TestClass", "priv");
reflectMethod("TestClass", "prot");
reflectMethod("DerivedClass", "prot");
reflectMethod("TestInterface", "int");
reflectMethod("ReflectionProperty", "__construct");
reflectMethod("TestClass", "__destruct");

?>
--EXPECTF--
**********************************
Reflecting on method DerivedClass::foo()


getFileName():
string(%d) "%sReflectionMethod_basic4.php"

getStartLine():
int(18)

getEndLine():
int(24)

**********************************
**********************************
Reflecting on method TestClass::stat()


getFileName():
string(%d) "%sReflectionMethod_basic4.php"

getStartLine():
int(26)

getEndLine():
int(28)

**********************************
**********************************
Reflecting on method TestClass::priv()


getFileName():
string(%d) "%sReflectionMethod_basic4.php"

getStartLine():
int(30)

getEndLine():
int(32)

**********************************
**********************************
Reflecting on method TestClass::prot()


getFileName():
string(%d) "%sReflectionMethod_basic4.php"

getStartLine():
int(34)

getEndLine():
int(34)

**********************************
**********************************
Reflecting on method DerivedClass::prot()


getFileName():
string(%d) "%sReflectionMethod_basic4.php"

getStartLine():
int(34)

getEndLine():
int(34)

**********************************
**********************************
Reflecting on method TestInterface::int()


getFileName():
string(%d) "%sReflectionMethod_basic4.php"

getStartLine():
int(42)

getEndLine():
int(42)

**********************************
**********************************
Reflecting on method ReflectionProperty::__construct()


getFileName():
bool(false)

getStartLine():
bool(false)

getEndLine():
bool(false)

**********************************
**********************************
Reflecting on method TestClass::__destruct()


getFileName():
string(%d) "%sReflectionMethod_basic4.php"

getStartLine():
int(36)

getEndLine():
int(36)

**********************************
