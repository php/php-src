--TEST--
Bug #48408 (crash when exception is thrown while passing function arguments)
--FILE--
<?php
class B{
	public function process($x){
		return $x;
	}
}
class C{
	public function generate($x){
		throw new Exception;
	}
}
$b = new B;
$c = new C;
try{
	$b->process($c->generate(0));
}
catch(Exception $e){
	$c->generate(0);
}
?>
--EXPECTF--

Fatal error: Uncaught exception 'Exception' in %s
Stack trace:
#0 %s(%d): C->generate(0)
#1 {main}
  thrown in %s
