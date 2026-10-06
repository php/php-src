--TEST--
.phpc: class definitions and namespaces work in a .phpc file
--FILE--
<?php
$file = __DIR__ . '/' . basename(__FILE__, '.php') . '.phpc';
file_put_contents($file, <<<'PHPC'
namespace App\Demo;

class Greeter {
    public function __construct(private string $who) {}
    public function hello(): string { return "hello, {$this->who}"; }
}

echo (new Greeter('world'))->hello(), "\n";
PHPC);
register_shutdown_function(fn() => @unlink($file));
require $file;
?>
--EXPECT--
hello, world
