--TEST--
Basic arrayof functions
--FILE--
<?php
interface IFace {}

class One implements IFace {}

class Two extends One {}

function test(IFace[] $objects) {
	return $objects;
}

var_dump(test([new Two(), new One(), new Two()]));

function test2(Callable[] $functions) {
	return $functions;
}

var_dump(test2([function(){}, function(){}]));

function test3(array[] $arrays) {
	return $arrays;
}

var_dump(test3([array(), array()]));

function test4(array[] $arrays, $something, IFace[] $faces) {
	return [$arrays, $something, $faces];
}

var_dump(test4([array(), array()], true, [new Two(), new One()]));

function test5(array[] ... $arrays) {
	return $arrays;
}

var_dump(test5([array(), array()], [array(), array()]));
?>
--EXPECT--
array(3) {
  [0]=>
  object(Two)#1 (0) {
  }
  [1]=>
  object(One)#2 (0) {
  }
  [2]=>
  object(Two)#3 (0) {
  }
}
array(2) {
  [0]=>
  object(Closure)#3 (0) {
  }
  [1]=>
  object(Closure)#2 (0) {
  }
}
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(0) {
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    array(0) {
    }
  }
  [1]=>
  bool(true)
  [2]=>
  array(2) {
    [0]=>
    object(Two)#2 (0) {
    }
    [1]=>
    object(One)#3 (0) {
    }
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    array(0) {
    }
  }
  [1]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    array(0) {
    }
  }
}

