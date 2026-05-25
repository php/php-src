--TEST--
GH-22118: spl_autoload_unregister() unregisters equivalent first-class method callables
--FILE--
<?php
class AutoloadTest
{
	public function load(string $class): void
	{
		echo "autoload $class\n";
	}

	public function __call(string $name, array $arguments): void
	{
		echo "autoload {$arguments[0]}\n";
	}

	public function run(): void
	{
		spl_autoload_register($this->load(...));
		var_dump(spl_autoload_unregister($this->load(...)));
		spl_autoload_call('MissingDirect');

		spl_autoload_register($this->missing(...));
		var_dump(spl_autoload_unregister($this->missing(...)));
		spl_autoload_call('MissingTrampoline');
	}
}

(new AutoloadTest())->run();
?>
--EXPECT--
bool(true)
bool(true)
