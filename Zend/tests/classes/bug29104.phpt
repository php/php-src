--TEST--
Bug #29104 (Function declaration in method doesn't work)
--FILE--
<?php
class A
{
  function g()
  {
    echo "function g - begin\n";

    function f()
    {
      echo "function f\n";
    }

    echo "function g - end\n";
  }
}

$a = new A;
$a->g();
f();
?>
--EXPECT--
function g - begin
function g - end
function f
