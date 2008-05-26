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
Notice: serialize(): "no_such" returned as member variable from __sleep() but does not exist in %s on line %d
unicode(130) "O:3:"foo":4:{U:12:"\0foo\0private";U:7:"private";U:12:"\0*\0protected";U:9:"protected";U:6:"public";U:6:"public";U:7:"no_such";N;}"
