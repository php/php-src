--TEST--
Test usage of ReflectionClassConstant methods __toString(), getName(), getValue(), isPublic(), isPrivate(), isProtected(), getModifiers(), getDeclaringClass() and getDocComment().
--FILE--
<?php

function reflectClassConstant($base, $constant) {
    $constInfo = new ReflectionClassConstant($base, $constant);
    echo "**********************************\n";
    $class = is_object($base) ? get_class($base) : $base;
    echo "Reflecting on class constant $class::$constant\n\n";
    echo "__toString():\n";
    var_dump($constInfo->__toString());
    echo "getName():\n";
    var_dump($constInfo->getName());
    echo "getValue():\n";
    var_dump($constInfo->getValue());
    echo "isPublic():\n";
    var_dump($constInfo->isPublic());
    echo "isPrivate():\n";
    var_dump($constInfo->isPrivate());
    echo "isProtected():\n";
    var_dump($constInfo->isProtected());
    echo "isFinal():\n";
    var_dump($constInfo->isFinal());
    echo "getModifiers():\n";
    var_dump($constInfo->getModifiers());
    echo "getDeclaringClass():\n";
    var_dump($constInfo->getDeclaringClass());
    echo "getDocComment():\n";
    var_dump($constInfo->getDocComment());
    echo "\n**********************************\n";
}

class TestClass {
    public const /** My Doc comment */ PUB = true;
    /** Another doc comment */
    protected const PROT = 4;
    private const PRIV = "keepOut";
    public final const FINAL = "foo";
}
$instance = new TestClass();

reflectClassConstant("TestClass", "PUB");
reflectClassConstant("TestClass", "PROT");
reflectClassConstant("TestClass", "PRIV");
reflectClassConstant("TestClass", "FINAL");
reflectClassConstant($instance, "PRIV");
reflectClassConstant($instance, "BAD_CONST");

?>
--EXPECTF--
**********************************
Reflecting on class constant TestClass::PUB

__toString():
string(35) "Constant [ public bool PUB ] { 1 }
"
getName():
string(3) "PUB"
getValue():
bool(true)
isPublic():
bool(true)
isPrivate():
bool(false)
isProtected():
bool(false)
isFinal():
bool(false)
getModifiers():
int(1)
getDeclaringClass():
object(ReflectionClass)#3 (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
string(21) "/** My Doc comment */"

**********************************
**********************************
Reflecting on class constant TestClass::PROT

__toString():
string(38) "Constant [ protected int PROT ] { 4 }
"
getName():
string(4) "PROT"
getValue():
int(4)
isPublic():
bool(false)
isPrivate():
bool(false)
isProtected():
bool(true)
isFinal():
bool(false)
getModifiers():
int(2)
getDeclaringClass():
object(ReflectionClass)#3 (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
string(26) "/** Another doc comment */"

**********************************
**********************************
Reflecting on class constant TestClass::PRIV

__toString():
string(45) "Constant [ private string PRIV ] { keepOut }
"
getName():
string(4) "PRIV"
getValue():
string(7) "keepOut"
isPublic():
bool(false)
isPrivate():
bool(true)
isProtected():
bool(false)
isFinal():
bool(false)
getModifiers():
int(4)
getDeclaringClass():
object(ReflectionClass)#3 (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
bool(false)

**********************************
**********************************
Reflecting on class constant TestClass::FINAL

__toString():
string(47) "Constant [ final public string FINAL ] { foo }
"
getName():
string(5) "FINAL"
getValue():
string(3) "foo"
isPublic():
bool(true)
isPrivate():
bool(false)
isProtected():
bool(false)
isFinal():
bool(true)
getModifiers():
int(33)
getDeclaringClass():
object(ReflectionClass)#3 (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
bool(false)

**********************************
**********************************
Reflecting on class constant TestClass::PRIV

__toString():
string(45) "Constant [ private string PRIV ] { keepOut }
"
getName():
string(4) "PRIV"
getValue():
string(7) "keepOut"
isPublic():
bool(false)
isPrivate():
bool(true)
isProtected():
bool(false)
isFinal():
bool(false)
getModifiers():
int(4)
getDeclaringClass():
object(ReflectionClass)#3 (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
bool(false)

**********************************

Fatal error: Uncaught ReflectionException: Constant TestClass::BAD_CONST does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClassConstant->__construct(Object(TestClass), 'BAD_CONST')
#1 %s(%d): reflectClassConstant(Object(TestClass), 'BAD_CONST')
#2 {main}
  thrown in %s on line %d
