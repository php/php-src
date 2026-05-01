--TEST--
nullable list<T> property accepts null and matching object list
--FILE--
<?php
class User {}

class Box {
    public ?list<User> $users = null;
}

$box = new Box;
var_dump($box->users);
$box->users = [new User];
echo count($box->users), "\n";
?>
--EXPECT--
NULL
1
