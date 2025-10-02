--TEST--
Bug #26737 (Protected and private variables are not saved on serialization when a user defined __sleep is used)
--FILE--
<?php
class foo
{
    private $private = 'private';
    protected $protected = 'protected';
    public $public = 'public';

    public function __sleep()
    {
        return array('private', 'protected', 'public', 'no_such');
    }
}
$foo = new foo();
$data = serialize($foo);
var_dump(str_replace("\0", '\0', $data));
?>
--EXPECTF--
Warning: serialize(): "no_such" returned as member variable from __sleep() but does not exist in %s on line %d
string(114) "O:3:"foo":3:{s:12:"\0foo\0private";s:7:"private";s:12:"\0*\0protected";s:9:"protected";s:6:"public";s:6:"public";}"
