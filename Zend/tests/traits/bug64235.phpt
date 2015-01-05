--TEST--
Bug #64235 (Insteadof not work for class method in 5.4.11)
--FILE--
<?php

class TestParentClass
{
    public function method()
    {
        print_r('Parent method');
        print "\n";
    }
}

trait TestTrait
{
    public function method()
    {
        print_r('Trait method');
        print "\n";
    }
}

class TestChildClass extends TestParentClass
{
    use TestTrait
    {
        TestTrait::method as methodAlias;
        TestParentClass::method insteadof TestTrait;
    }
}
?>
--EXPECTF--
Fatal error: Class TestParentClass is not a trait, Only traits may be used in 'as' and 'insteadof' statements in %sbug64235.php on line %d
