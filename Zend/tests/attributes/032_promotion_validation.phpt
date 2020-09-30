--TEST--
Validation of attributes on promoted properties
--FILE--
<?php

#[Attribute(Attribute::TARGET_PROPERTY)]
class PropOnly {
}

#[Attribute(Attribute::TARGET_PARAMETER)]
class ParamOnly {
}

#[Attribute(Attribute::TARGET_CLASS)]
class ClassOnly {
}

class Test {
    public function __construct(
        #[PropOnly]
        public $propOnly,
        #[ParamOnly]
        public $paramOnly,
        #[ClassOnly]
        public $classOnly,
    ) {}
}

$rm = new ReflectionMethod(Test::class, '__construct');
$ra1 = $rm->getParameters()[0]->getAttributes()[0];
var_dump($ra1->newInstance());
$ra2 = $rm->getParameters()[1]->getAttributes()[0];
var_dump($ra2->newInstance());
try {
    $ra3 = $rm->getParameters()[2]->getAttributes()[0];
    var_dump($ra3->newInstance());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
echo "\n";

$rc = new ReflectionClass(Test::class);
$ra3 = $rc->getProperties()[0]->getAttributes()[0];
var_dump($ra3->newInstance());
$ra4 = $rc->getProperties()[1]->getAttributes()[0];
var_dump($ra4->newInstance());
try {
    $ra5 = $rc->getProperties()[2]->getAttributes()[0];
    var_dump($ra5->newInstance());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(PropOnly)#2 (0) {
}
object(ParamOnly)#3 (0) {
}
Attribute "ClassOnly" cannot target parameter (allowed targets: class)

object(PropOnly)#2 (0) {
}
object(ParamOnly)#7 (0) {
}
Attribute "ClassOnly" cannot target property (allowed targets: class)
