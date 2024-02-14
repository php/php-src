--TEST--
GH-10026 (Garbage collection stops after exception in SoapClient)
--EXTENSIONS--
soap
--FILE--
<?php

class A
{
    public $b;
}

class B
{
    public $a;
}

function buildCycle()
{
    $a = new A();
    $b = new B();
    $a->b = $b;
    $b->a = $a;
}

function failySoapCall()
{
  try {
      new SoapClient('https://127.0.0.1/?WSDL');
  } catch (Exception) {
      echo "Soap call failed\n";
  }
}

function gc()
{
	gc_collect_cycles();
	echo "GC Runs: " . gc_status()['runs'] . "\n";
}


buildCycle();
gc();
buildCycle();
gc();
buildCycle();
gc();

failySoapCall();

buildCycle();
gc();
buildCycle();
gc();
buildCycle();
gc();
?>
--EXPECT--
GC Runs: 1
GC Runs: 2
GC Runs: 3
Soap call failed
GC Runs: 4
GC Runs: 5
GC Runs: 6
