--TEST--
Invalid SplFileObject::getCurrentLine() return type
--FILE--
<?php

class MySplFileObject extends SplFileObject {
    #[ReturnTypeWillChange]
    public function getCurrentLine(): array {
        return [1, 2, 3];
    }
}

$obj = new MySplFileObject(__FILE__);
try {
    var_dump($obj->current());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
MySplFileObject::getCurrentLine(): Return value must be of type string, array returned
