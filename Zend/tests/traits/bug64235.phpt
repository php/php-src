--TEST--
Bug #64235 (Insteadof not work for class method in 5.4.11)
--SKIPIF--
<?php
if (!version_compare(zend_version(), '2.5.0', '<')) die('skip ZendEngine 2.4 only'); ?>
?>
--FILE--
<?php

class TestParentClass
{
    public function method()
    {
        print_r('Parent method');
        print "\n";
    }

	public function bar()
	{
		print "bar\n";
	}
}

trait TestTrait
{
    public function method()
    {
        print_r('Trait method');
        print "\n";
    }

	public function bar()
	{
		print "bar trait";
	}
}

class TestChildClass extends TestParentClass
{
    use TestTrait
    {
        TestTrait::method as methodAlias;

		/* They should be traits but not classes */
        TestTrait::bar insteadof TestParentClass;
		TestParentClass::bar as newbar;
        TestParentClass::method insteadof TestTrait;
    }
}

(new TestChildClass)->method();
(new TestChildClass)->methodAlias();
echo "==DONE=="
?>
--EXPECTF--
Deprecated: Using class as trait is deprecated, class: TestParentClass is not a trait in %sbug64235.php on line %d

Deprecated: Using class as trait is deprecated, class: TestParentClass is not a trait in %sbug64235.php on line %d

Deprecated: Using class as trait is deprecated, class: TestParentClass is not a trait in %sbug64235.php on line %d
Parent method
Trait method
==DONE==
