--TEST--
ReflectionParameter::getClass(), getDeclaringClass(), getDeclaringFunction()
--FILE--
<?php

function test($nix, Array $ar, &$ref, stdClass $std,
    NonExistingClass $na, stdClass &$opt = NULL, $def = "FooBar")
{
}

class test
{
    function method($nix, Array $ar, &$ref, stdClass $std,
        NonExistingClass $na, stdClass $opt = NULL, $def = "FooBar")
    {
    }
}

function check_params_decl_func($r, $f)
{
    $c = $r->$f();
    $sep = $c instanceof ReflectionMethod ? $c->class . '::' : '';
    echo $f . ': ' . ($c ? $sep . $c->name : 'NULL') . "()\n";
}

function check_params_decl_class($r, $f)
{
    $c = $r->$f();
    echo $f . ': ' . ($c ? $c->name : 'NULL') . "\n";
}

function check_params_func($r, $f)
{
    echo $f . ': ';
    $v = $r->$f();
    var_dump($v);
}

function check_params($r)
{
    echo "#####" . ($r instanceof ReflectionMethod ? $r->class . '::' : '') . $r->name . "()#####\n";
    $i = 0;
    foreach($r->getParameters() as $p)
    {
        echo "===" . $i . "===\n";
        $i++;
        check_params_func($p, 'getName');
        check_params_func($p, 'isPassedByReference');
        try
        {
            check_params_decl_class($p, 'getClass');
        }
        catch(ReflectionException $e)
        {
            echo $e->getMessage() . "\n";
        }
        check_params_decl_class($p, 'getDeclaringClass');
//		check_params_decl_func($p, 'getDeclaringFunction');
        check_params_func($p, 'isArray');
        check_params_func($p, 'allowsNull');
        check_params_func($p, 'isOptional');
        check_params_func($p, 'isDefaultValueAvailable');
        if ($p->isOptional())
        {
            check_params_func($p, 'getDefaultValue');
        }
    }
}

check_params(new ReflectionFunction('test'));

check_params(ReflectionMethod::createFromMethodName('test::method'));

?>
--EXPECTF--
#####test()#####
===0===
getName: string(3) "nix"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: NULL
getDeclaringClass: NULL
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(true)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===1===
getName: string(2) "ar"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: NULL
getDeclaringClass: NULL
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(true)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===2===
getName: string(3) "ref"
isPassedByReference: bool(true)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: NULL
getDeclaringClass: NULL
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(true)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===3===
getName: string(3) "std"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: stdClass
getDeclaringClass: NULL
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===4===
getName: string(2) "na"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
Class "NonExistingClass" does not exist
getDeclaringClass: NULL
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===5===
getName: string(3) "opt"
isPassedByReference: bool(true)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: stdClass
getDeclaringClass: NULL
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(true)
isOptional: bool(true)
isDefaultValueAvailable: bool(true)
getDefaultValue: NULL
===6===
getName: string(3) "def"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: NULL
getDeclaringClass: NULL
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(true)
isOptional: bool(true)
isDefaultValueAvailable: bool(true)
getDefaultValue: string(6) "FooBar"
#####test::method()#####
===0===
getName: string(3) "nix"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: NULL
getDeclaringClass: test
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(true)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===1===
getName: string(2) "ar"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: NULL
getDeclaringClass: test
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(true)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===2===
getName: string(3) "ref"
isPassedByReference: bool(true)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: NULL
getDeclaringClass: test
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(true)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===3===
getName: string(3) "std"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: stdClass
getDeclaringClass: test
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===4===
getName: string(2) "na"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
Class "NonExistingClass" does not exist
getDeclaringClass: test
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===5===
getName: string(3) "opt"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: stdClass
getDeclaringClass: test
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(true)
isOptional: bool(true)
isDefaultValueAvailable: bool(true)
getDefaultValue: NULL
===6===
getName: string(3) "def"
isPassedByReference: bool(false)

Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
getClass: NULL
getDeclaringClass: test
isArray: 
Deprecated: Method ReflectionParameter::isArray() is deprecated in %s on line %d
bool(false)
allowsNull: bool(true)
isOptional: bool(true)
isDefaultValueAvailable: bool(true)
getDefaultValue: string(6) "FooBar"
