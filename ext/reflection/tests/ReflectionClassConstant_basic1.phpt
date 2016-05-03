--TEST--
Test usage of ReflectionClassConstant methods __toString(), export(), getName(), getValue(), isPublic(), isPrivate(), isProtected(), getModifiers(), getDeclaringClass() and getDocComment().
--FILE--
<?php

function reflectClassConstant($base, $constant) {
    $constInfo = new ReflectionClassConstant($base, $constant);
    echo "**********************************\n";
    $class = is_object($base) ? get_class($base) : $base;
    echo "Reflecting on class constant $class::$constant\n\n";
    echo "__toString():\n";
    var_dump($constInfo->__toString());
    echo "export():\n";
    var_dump(ReflectionClassConstant::export($base, $constant, true));
    echo "export():\n";
    var_dump(ReflectionClassConstant::export($base, $constant, false));
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
}
$instance = new TestClass();

reflectClassConstant("TestClass", "PUB");
reflectClassConstant("TestClass", "PROT");
reflectClassConstant("TestClass", "PRIV");
reflectClassConstant($instance, "PRIV");
reflectClassConstant($instance, "BAD_CONST");

?>
--EXPECTF--
**********************************
Reflecting on class constant TestClass::PUB

__toString():
string(38) "Constant [ public boolean PUB ] { 1 }
"
export():
string(38) "Constant [ public boolean PUB ] { 1 }
"
export():
Constant [ public boolean PUB ] { 1 }

NULL
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
getModifiers():
int(256)
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
string(42) "Constant [ protected integer PROT ] { 4 }
"
export():
string(42) "Constant [ protected integer PROT ] { 4 }
"
export():
Constant [ protected integer PROT ] { 4 }

NULL
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
getModifiers():
int(512)
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
export():
string(45) "Constant [ private string PRIV ] { keepOut }
"
export():
Constant [ private string PRIV ] { keepOut }

NULL
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
getModifiers():
int(1024)
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
export():
string(45) "Constant [ private string PRIV ] { keepOut }
"
export():
Constant [ private string PRIV ] { keepOut }

NULL
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
getModifiers():
int(1024)
getDeclaringClass():
object(ReflectionClass)#3 (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
bool(false)

**********************************

Fatal error: Uncaught ReflectionException: Class Constant TestClass::BAD_CONST does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClassConstant->__construct(Object(TestClass), 'BAD_CONST')
#1 %s(%d): reflectClassConstant(Object(TestClass), 'BAD_CONST')
#2 {main}
  thrown in %s on line %d
