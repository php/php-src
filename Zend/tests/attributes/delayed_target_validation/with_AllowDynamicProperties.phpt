--TEST--
#[\DelayedTargetValidation] with #[\AllowDynamicProperties]: invalid targets don't error
--FILE--
<?php

#[DelayedTargetValidation]
#[AllowDynamicProperties] // Does something here
class DemoClass {
  #[DelayedTargetValidation]
  #[AllowDynamicProperties] // Does nothing here
  public $val;

  #[DelayedTargetValidation]
  #[AllowDynamicProperties] // Does nothing here
  public const CLASS_CONST = 'FOO';

  public function __construct(
    #[DelayedTargetValidation]
    #[AllowDynamicProperties] // Does nothing here
    $str
  ) {
    echo "Got: $str\n";
    $this->val = $str;
  }

  #[DelayedTargetValidation]
  #[AllowDynamicProperties] // Does nothing here
  public function printVal() {
    echo 'Value is: ' . $this->val . "\n";
  }

}

#[DelayedTargetValidation]
#[AllowDynamicProperties] // Does nothing here
function demoFn() {
  echo __FUNCTION__ . "\n";
  return 456;
}

#[DelayedTargetValidation]
#[AllowDynamicProperties] // Does nothing here
const GLOBAL_CONST = 'BAR';

$d = new DemoClass('example');
$d->printVal();
var_dump($d->val);
var_dump(DemoClass::CLASS_CONST);
demoFn();
var_dump(GLOBAL_CONST);

$d->missingProp = 'foo';
var_dump($d);
?>
--EXPECTF--
Got: example
Value is: example
string(7) "example"
string(3) "FOO"
demoFn
string(3) "BAR"
object(DemoClass)#%d (2) {
  ["val"]=>
  string(7) "example"
  ["missingProp"]=>
  string(3) "foo"
}
