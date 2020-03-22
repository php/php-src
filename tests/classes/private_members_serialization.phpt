--TEST--
Verifies that it is possible to return private member names of parent classes in __sleep
--FILE--
<?php
class foo
{
    private $private = 'private';
    protected $protected = 'protected';
    public $public = 'public';
}

class bar extends foo
{
    public function __sleep()
    {
        return array("\0foo\0private", 'protected', 'public');
    }
}

var_dump(str_replace("\0", '\0', serialize(new bar())));
?>
--EXPECT--
string(114) "O:3:"bar":3:{s:12:"\0foo\0private";s:7:"private";s:12:"\0*\0protected";s:9:"protected";s:6:"public";s:6:"public";}"
