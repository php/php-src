--TEST--
list<T> property accepts matching object list and rejects bad element
--FILE--
<?php
class User {}

class Box {
    public list<User> $users;
}

$box = new Box;
$box->users = [new User, new User];
echo count($box->users), "\n";

try {
    $box->users = [new stdClass];
} catch (TypeError $e) {
    echo "TypeError\n";
}
?>
--EXPECT--
2
TypeError
