--TEST--
Bug #34277 (array_filter() crashes with references and objects)
--FILE--
<?php

class C
{
  function m1()
  {
    $this->m2();
  }

  function m2()
  {
    $this->m3();
  }

  function m3()
  {
    $this->m4();
  }

  function m4()
  {
    $this->m5();
  }

  function m5()
  {
    $this->m6();
  }

  function m6()
  {
    $this->m7();
  }

  function m7()
  {
    $this->m8();
  }

  function m8()
  {
    $this->m9();
  }

  function m9()
  {
    $this->m10();
  }

  function m10()
  {
    $this->m11(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
  }

  function m11($a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8, $a9, $a10)
  {
    $arr = explode('a', 'b');
  }
}

function f($str)
{
  $obj = new C;
  $obj->m1();
  return TRUE;
}

function p5($a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8, $a9, $a10, $a11, $a12)
{
  $ret = array_filter(array(0), 'f');
}

function p4()
{
  p5(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
}

function p3()
{
  p4();
}

function p2()
{
  p3();
}

function p1()
{
  p2();
}

p1();
echo "ok\n";
?>
--EXPECT--
ok
