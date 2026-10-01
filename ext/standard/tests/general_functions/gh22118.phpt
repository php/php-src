--TEST--
GH-22118: unregister_tick_function() unregisters equivalent first-class method callables
--FILE--
<?php
declare(ticks=1);

$string = 0;

function tick_string(): void
{
	global $string;
	$string++;
}

class TickTest
{
	public int $direct = 0;
	public int $trampoline = 0;
	public int $array = 0;
	public int $invoke = 0;

	public function kick(): void
	{
		$this->direct++;
	}

	public function arrayCallable(): void
	{
		$this->array++;
	}

	public function __invoke(): void
	{
		$this->invoke++;
	}

	public function __call(string $name, array $arguments): void
	{
		$this->trampoline++;
	}

	public function run(): void
	{
		register_tick_function($this->kick(...));
		unregister_tick_function($this->kick(...));
		echo "direct: {$this->direct}\n";

		register_tick_function($this->missing(...));
		unregister_tick_function($this->missing(...));
		echo "trampoline: {$this->trampoline}\n";

		register_tick_function($this->arrayCallable(...));
		unregister_tick_function([$this, 'arrayCallable']);
		echo "array: {$this->array}\n";

		register_tick_function($this(...));
		unregister_tick_function($this);
		echo "invoke: {$this->invoke}\n";
	}
}

register_tick_function(tick_string(...));
unregister_tick_function('tick_string');
echo "string: {$string}\n";

(new TickTest())->run();
echo "done\n";
?>
--EXPECT--
string: 1
direct: 1
trampoline: 1
array: 1
invoke: 1
done
