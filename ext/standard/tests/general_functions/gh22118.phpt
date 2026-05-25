--TEST--
GH-22118: unregister_tick_function() unregisters equivalent first-class method callables
--FILE--
<?php
declare(ticks=1);

class TickTest
{
	public int $direct = 0;
	public int $trampoline = 0;

	public function kick(): void
	{
		$this->direct++;
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
	}
}

(new TickTest())->run();
echo "done\n";
?>
--EXPECT--
direct: 1
trampoline: 1
done
