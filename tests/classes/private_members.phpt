--TEST--
ZE2 A private member is
--FILE--
<?php

class base
{
  private $member;

  function __construct()
  {
    echo __METHOD__ . "(begin)\n";
    $this->member = 'base::member';
    $this->test();
    echo __METHOD__ . "(end)\n";
  }

  function test()
  {
    echo __METHOD__ . "\n";
    print_r($this);
  }
}

class derived extends base
{
  public $member = 'derived::member (default)';

  function __construct()
  {
    echo __METHOD__ . "(begin)\n";
    parent::__construct();
    parent::test();
    $this->test();
    $this->member = 'derived::member';
    echo __METHOD__ . "(end)\n";
  }

  function test()
  {
    parent::test();
    echo __METHOD__ . "\n";
    print_r($this);
  }
}

$t = new derived;
$t->test();
unset($t);

echo "Done\n";

?>
--EXPECT--
derived::__construct(begin)
base::__construct(begin)
base::test
derived Object
(
    [member] => derived::member (default)
    [member:base:private] => base::member
)
derived::test
derived Object
(
    [member] => derived::member (default)
    [member:base:private] => base::member
)
base::__construct(end)
base::test
derived Object
(
    [member] => derived::member (default)
    [member:base:private] => base::member
)
base::test
derived Object
(
    [member] => derived::member (default)
    [member:base:private] => base::member
)
derived::test
derived Object
(
    [member] => derived::member (default)
    [member:base:private] => base::member
)
derived::__construct(end)
base::test
derived Object
(
    [member] => derived::member
    [member:base:private] => base::member
)
derived::test
derived Object
(
    [member] => derived::member
    [member:base:private] => base::member
)
Done
