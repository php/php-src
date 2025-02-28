--TEST--
Optional interfaces doesn't make Override optional
--FILE--
<?php

interface ExistingInterface
{
    public function method();
}

class TestClass implements ?ExistingInterface, ?NonExistantInterface
{
    #[\Override]
    public function method() {}

    #[\Override]
    public function other() {}
}

?>
--EXPECTF--
Fatal error: TestClass::other() has #[\Override] attribute, but no matching parent method exists in %soverride.php on line %d
