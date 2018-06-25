--TEST--
Friendship is not transitive. If Base is a friend of another class, Derived is not automatically a friend.
--FILE--
<?php

class Subject
{
	friend Base;

	protected $property = 'subject';
}

class Base
{
	protected $friend;

	public function __construct(Subject $subject)
	{
		$this->friend = $subject;
	}
}

class Derived extends Base
{
	public function touch()
	{
		echo $this->friend->property . PHP_EOL;
	}
}

$subject = new Subject();
$derived = new Derived($subject);

$derived->touch();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access protected property Subject::$property in /var/www/html/php-src/Zend/tests/friends/friend_inheritance_002.php:%d
Stack trace:
#0 /var/www/html/php-src/Zend/tests/friends/friend_inheritance_002.php(%d): Derived->touch()
#1 {main}
  thrown in /var/www/html/php-src/Zend/tests/friends/friend_inheritance_002.php on line %d