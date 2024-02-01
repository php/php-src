--TEST--
A SensitiveParameterValue keeps inner objects alive.
--FILE--
<?php

class CustomDestructor
{
  public function __construct(private int $id)
  {
    echo __METHOD__, " - ", $this->id, PHP_EOL;
  }

  public function __destruct()
  {
    echo __METHOD__, " - ", $this->id, PHP_EOL;
  }

  public function getId(): int
  {
    return $this->id;
  }
}

function test(#[SensitiveParameter] CustomDestructor $o, bool $throw)
{
  if ($throw) {
    throw new Exception('Error');
  }
}

function wrapper(int $id, bool $throw)
{
  $o = new CustomDestructor($id);
  test($o, $throw);
}

function main(): SensitiveParameterValue
{
  try {
    echo "Before 1", PHP_EOL;
    wrapper(1, false);
    echo "After 1", PHP_EOL;
    echo "Before 2", PHP_EOL;
    wrapper(2, true);
    echo "Not Reached: After 2", PHP_EOL;
  } catch (Exception $e) {
    echo "catch", PHP_EOL;
    return $e->getTrace()[0]['args'][0];
  }
}

$v = main();

var_dump($v->getValue()->getId());

echo "Before unset", PHP_EOL;

unset($v);

echo "After unset", PHP_EOL;

?>
--EXPECT--
Before 1
CustomDestructor::__construct - 1
CustomDestructor::__destruct - 1
After 1
Before 2
CustomDestructor::__construct - 2
catch
int(2)
Before unset
CustomDestructor::__destruct - 2
After unset
