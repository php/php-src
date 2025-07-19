--TEST--
#[\DelayedTargetValidation] with #[\Override]: invalid targets or actual overrides don't do anything
--FILE--
<?php

class Base {
  public function printVal() {
    echo __METHOD__ . "\n";
  }
}

#[DelayedTargetValidation]
#[Override] // Does nothing here
class DemoClass extends Base {
  #[DelayedTargetValidation]
  #[Override] // Does nothing here
  public $val;

  #[DelayedTargetValidation]
  #[Override] // Does nothing here
  public const CLASS_CONST = 'FOO';

  public function __construct(
    #[DelayedTargetValidation]
    #[Override] // Does nothing here
    $str
  ) {
    echo "Got: $str\n";
    $this->val = $str;
  }

  #[DelayedTargetValidation]
  #[Override] // Does something here
  public function printVal() {
    echo 'Value is: ' . $this->val . "\n";
    return 123;
  }
}

#[DelayedTargetValidation]
#[Override] // Does nothing here
function demoFn() {
  echo __FUNCTION__ . "\n";
  return 456;
}

#[DelayedTargetValidation]
#[Override] // Does nothing here
const GLOBAL_CONST = 'BAR';

$d = new DemoClass('example');
$d->printVal();
var_dump($d->val);
var_dump(DemoClass::CLASS_CONST);
demoFn();
var_dump(GLOBAL_CONST);
?>
--EXPECT--
Got: example
Value is: example
string(7) "example"
string(3) "FOO"
demoFn
string(3) "BAR"
