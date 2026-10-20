--TEST--
unserialize() with allowed_classes turns C: objects of classes that are not allowed into incomplete objects
--FILE--
<?php

final class Kept
{
    public $a = 1;
}

final class Blocked implements Serializable
{
    public function serialize(): string
    {
        return '';
    }

    public function unserialize(string $data): void
    {
        echo __METHOD__, "\n";
    }

    public function __serialize(): array
    {
        return [];
    }

    public function __unserialize(array $data): void
    {
        echo __METHOD__, "\n";
    }
}

$payload = 'a:4:{i:0;O:4:"Kept":1:{s:1:"a";i:2;}i:1;C:7:"Blocked":4:{abcd}i:2;C:7:"Missing":4:{abcd}i:3;r:4;}';

var_dump(unserialize($payload, ['allowed_classes' => ['Kept']]));
?>
--EXPECTF--
Warning: Class __PHP_Incomplete_Class has no unserializer in %s on line %d

Warning: Class __PHP_Incomplete_Class has no unserializer in %s on line %d
array(4) {
  [0]=>
  object(Kept)#%d (1) {
    ["a"]=>
    int(2)
  }
  [1]=>
  object(__PHP_Incomplete_Class)#%d (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(7) "Blocked"
  }
  [2]=>
  object(__PHP_Incomplete_Class)#%d (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(7) "Missing"
  }
  [3]=>
  object(__PHP_Incomplete_Class)#%d (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(7) "Blocked"
  }
}
