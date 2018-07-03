--TEST--
Access due to friendship is inherited
--FILE--
<?php

class Base
{
	private $secret = 'secret';
	protected $override = 'override';
}

class Derived extends Base
{
	friend Friendly;

	protected $derived = 'derived';

	public function touch()
	{
		echo $this->secret . PHP_EOL;
    }
}

class Friendly
{
	public function touch(Derived $derived)
	{
		echo $derived->derived . PHP_EOL;    // Allowed via normal friend functionality
		echo $derived->override . PHP_EOL;   // Allowed because inherited and accessible by Derived

		echo $derived->secret . PHP_EOL;     // Dis-allowed because not accessible by Derived
											 // In this case, it will behave just as if the derived itself
											 // attempted to access the private property from base
	}
}

$derived = new Derived();

$derived->touch();
(new Friendly())->touch($derived);

?>
--EXPECTF--
Notice: Undefined property: Derived::$secret in %s on line %d

derived
override

Notice: Undefined property: Derived::$secret in %s on line %d