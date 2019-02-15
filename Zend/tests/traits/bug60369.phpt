--TEST--
Bug #60369 (Crash with static property in trait)
--FILE--
<?php

trait PropertiesTrait {
   static $same = true;
}

class Properties {
   use PropertiesTrait;
   public $same = true;
}

?>
--EXPECTF--
Fatal error: Properties and PropertiesTrait define the same property ($same) in the composition of Properties. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
