--TEST--
Bug #49649 (unserialize() doesn't handle changes in property visibility) - to public
--FILE--
<?php

/**
 *class Foo
 *{
 *	private $private = 1;
 *
 *	protected $protected = 2;
 *
 *	public $public = 3;
 *
 *	public $notThere = 'old';
 * }
 *
 * echo base64_encode(serialize(new Foo()));
 *
 * The class above represents the serialized, base64_encoded string below.
*/
$serialized = 'TzozOiJGb28iOjQ6e3M6MTI6IgBGb28AcHJpdmF0ZSI7aToxO3M6MTI6IgAqAHByb3RlY3RlZCI7aToyO3M6NjoicHVibGljIjtpOjM7czo4OiJub3RUaGVyZSI7czozOiJvbGQiO30';

class Foo
{
	public $public = null;

	public $protected = null;

	public $private = null;
}

$class = unserialize(base64_decode($serialized));
var_dump($class);
--EXPECT--
object(Foo)#1 (4) {
  ["public"]=>
  int(3)
  ["protected"]=>
  int(2)
  ["private"]=>
  int(1)
  ["notThere"]=>
  string(3) "old"
}
