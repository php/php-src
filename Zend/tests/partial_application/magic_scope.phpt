--TEST--
Magic method scope
--CREDITS--
Ryan @ Calif.io
--FILE--
<?php

class InstanceTarget
{
    private function secret(string $value): void
    {
        echo "PRIVATE-INSTANCE:$value\n";
    }

    public function __call(string $name, array $arguments): void
    {
        echo "MAGIC-INSTANCE:$name:" . implode(',', $arguments) . "\n";
    }
}

class StaticTarget
{
    private static function secret(string $value): void
    {
        echo "PRIVATE-STATIC:$value\n";
    }

    public static function __callStatic(string $name, array $arguments): void
    {
        echo "MAGIC-STATIC:$name:" . implode(',', $arguments) . "\n";
    }
}

$instance = new InstanceTarget();

$instance->secret('direct');
StaticTarget::secret('direct');

$instancePartial = $instance->secret(?);
$staticPartial = StaticTarget::secret(?);
$instancePartial('controlled');
$staticPartial('controlled');

?>
--EXPECT--
MAGIC-INSTANCE:secret:direct
MAGIC-STATIC:secret:direct
MAGIC-INSTANCE:secret:controlled
MAGIC-STATIC:secret:controlled
