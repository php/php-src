--TEST--
Bug #24499 (bogus handling of a public property as a private one)
--FILE--
<?php
class Id {
        private $id="priv";

        public function tester($obj)
        {
                $obj->id = "bar";
        }
}

$id = new Id();
$obj = new stdClass;
$obj->foo = "bar";
$id->tester($obj);
print_r($obj);
?>
--EXPECT--
stdClass Object
(
    [foo] => bar
    [id] => bar
)
