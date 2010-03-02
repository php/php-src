--TEST--
Bug #51176 (Static calling in non-static method behaves like $this->)
--FILE--
<?php
class Foo
{
	public function start()
	{
		self::bar();
		static::bar();
		Foo::bar();
	}
	
	public function __call($n, $a)
	{
		echo "instance\n";
	}
	
	public static function __callStatic($n, $a)
	{
		echo "static\n";
	}
}

$foo = new Foo();
$foo->start();

?>
--EXPECT--
static
static
static
