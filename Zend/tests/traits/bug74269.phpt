--TEST--
Bug #74269: It's possible to override trait property with different loosely-equal value
--FILE--
<?php
trait PropertiesTrait
{
    public $same = true;
}

class PropertiesExample
{
    use PropertiesTrait;
    public $same = 2;
}
?>
--EXPECTF--
Fatal error: PropertiesExample and PropertiesTrait define the same property ($same) in the composition of PropertiesExample. However, the definition differs and is considered incompatible. Class was composed in %s
