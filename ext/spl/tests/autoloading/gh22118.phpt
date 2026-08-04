--TEST--
GH-22118: spl_autoload_unregister() unregisters equivalent first-class method callables
--FILE--
<?php
function autoload_string(string $class): void
{
	echo "autoload $class\n";
}

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

	public function __invoke(string $class): void
	{
		echo "autoload $class\n";
	}

	public function run(): void
	{
		spl_autoload_register($this->load(...));
		var_dump(spl_autoload_unregister($this->load(...)));
		spl_autoload_call('MissingDirect');

		spl_autoload_register($this->missing(...));
		var_dump(spl_autoload_unregister($this->missing(...)));
		spl_autoload_call('MissingTrampoline');

		spl_autoload_register($this->load(...));
		var_dump(spl_autoload_unregister([$this, 'load']));
		spl_autoload_call('MissingArray');

		spl_autoload_register($this(...));
		var_dump(spl_autoload_unregister($this));
		spl_autoload_call('MissingInvokable');
	}
}

spl_autoload_register(autoload_string(...));
var_dump(spl_autoload_unregister('autoload_string'));
spl_autoload_call('MissingString');

(new AutoloadTest())->run();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
