--TEST--
#[\DelayedTargetValidation] with #[\SensitiveParameter]: parameter still redacted
--FILE--
<?php

#[DelayedTargetValidation]
#[SensitiveParameter] // Does nothing here
class DemoClass {
  #[DelayedTargetValidation]
  #[SensitiveParameter] // Does nothing here
  public $val;

  #[DelayedTargetValidation]
  #[SensitiveParameter] // Does nothing here
  public const CLASS_CONST = 'FOO';

  public function __construct(
    #[DelayedTargetValidation]
    #[SensitiveParameter] // Does something here
    $str
  ) {
    echo "Got: $str\n";
    $this->val = $str;
  }

  #[DelayedTargetValidation]
  #[SensitiveParameter] // Does nothing here
  public function printVal(
    #[DelayedTargetValidation]
    #[SensitiveParameter]
    $sensitive
  ) {
    throw new Exception('Testing backtrace');
  }

}

#[DelayedTargetValidation]
#[SensitiveParameter] // Does nothing here
function demoFn() {
  echo __FUNCTION__ . "\n";
  return 456;
}

#[DelayedTargetValidation]
#[SensitiveParameter] // Does nothing here
const GLOBAL_CONST = 'BAR';

$d = new DemoClass('example');
var_dump($d->val);
var_dump(DemoClass::CLASS_CONST);
demoFn();
var_dump(GLOBAL_CONST);

$d->printVal('BAZ');


?>
--EXPECTF--
Got: example
string(7) "example"
string(3) "FOO"
demoFn
string(3) "BAR"

Fatal error: Uncaught Exception: Testing backtrace in %s:%d
Stack trace:
#0 %s(%d): DemoClass->printVal(Object(SensitiveParameterValue))
#1 {main}
  thrown in %s on line %d
