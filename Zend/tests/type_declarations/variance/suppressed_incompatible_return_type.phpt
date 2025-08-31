--TEST--
Test that the notice can be suppressed when the return type/value of the overriding method is incompatible with the tentative return type/value of the overridden method
--FILE--
<?php

class MyDateTime extends DateTime
{
    /**
     * @return DateTime|false
     */
    #[ReturnTypeWillChange]
    public function modify(string $modifier) {
        return false;
    }
}

$date = new MyDateTime("2021-01-01 00:00:00");
var_dump($date->modify("+1 sec"));
?>
--EXPECT--
bool(false)
