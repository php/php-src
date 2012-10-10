--TEST--
Bug #33512 (unset() overloaded properties doesn't work)
--FILE--
<?php
class TheObj {
		public $RealVar1, $RealVar2, $RealVar3, $RealVar4;
		public $Var = array();

		function __set($var, $val) {
			$this->Var[$var] = $val;
		}
		function __get($var) {
			if(isset($this->Var[$var])) return $this->Var[$var];
			else return -1;
		}
		function __unset($var) {
			unset($this->Var[$var]);
		}
	}

	$SomeObj = new TheObj;

	// this will fine
	$SomeObj->RealVar1 = 'somevalue';
	$SomeObj->{'RealVar2'} = 'othervalue';
	$SomeObj->{'RealVar'.(3)} = 'othervaluetoo';
	$SomeObj->{'RealVar'.'4'} = 'anothervalue';

	// this will fine too
	$SomeObj->Virtual1 = 'somevalue';
	$SomeObj->{'Virtual2'} = 'othervalue';

	// it's can't be used since this will encounter error
	$SomeObj->{'Virtual'.(3)} = 'othervaluetoo';
	$SomeObj->{'Virtual'.'4'} = 'anothervalue';

	// but this will fine, ofcourse
	$SomeObj->Var['Virtual'.(3)] = 'othervaluetoo';
	$SomeObj->Var['Virtual'.'4'] = 'anothervalue';


	var_dump($SomeObj->RealVar1);
	print $SomeObj->{'RealVar'.(3)}."\n";
	
	unset($SomeObj->RealVar1);
	unset($SomeObj->{'RealVar'.(3)});

	//the lines below will catch by '__get' magic method since these variables are unavailable anymore
	var_dump($SomeObj->RealVar1);
	print $SomeObj->{'RealVar'.(3)}."\n";

	// now we will try to unset these variables
	unset($SomeObj->Virtual1);
	unset($SomeObj->{'Virtual'.(3)});

	//but, these variables are still available??? eventhough they're "unset"-ed
	print $SomeObj->Virtual1."\n";
	print $SomeObj->{'Virtual'.(3)}."\n";
?>
--EXPECT--
string(9) "somevalue"
othervaluetoo
int(-1)
-1
-1
-1
