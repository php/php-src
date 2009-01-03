--TEST--
Closure 034: var_dump() of a Closure
--FILE--
<?php

$outer = 25;

class Test {
	public $func1;
	public $var = 42;
	function __construct() {
		global $outer;
		$this->func1 = function($param, $other = "default") use ($outer) {
		};
	}
}

$o = new Test;
var_dump($o->func1);

$o->func2 = function($param, $other = "default") use ($outer) {
};

var_dump($o->func2);

$func3 = function($param, $other = "default") use ($outer) {
};

var_dump($func3);

?>
===DONE===
--EXPECTF--
object(Closure)#%d (3) {
  ["this"]=>
  object(Test)#%d (2) {
    [u"func1"]=>
    object(Closure)#%d (3) {
      ["this"]=>
      object(Test)#%d (2) {
        [u"func1"]=>
        object(Closure)#%d (3) {
          ["this"]=>
          *RECURSION*
          ["static"]=>
          array(1) {
            [u"outer"]=>
            int(25)
          }
          ["parameter"]=>
          array(2) {
            ["$param"]=>
            string(10) "<required>"
            ["$other"]=>
            string(10) "<optional>"
          }
        }
        [u"var"]=>
        int(42)
      }
      ["static"]=>
      array(1) {
        [u"outer"]=>
        int(25)
      }
      ["parameter"]=>
      array(2) {
        ["$param"]=>
        string(10) "<required>"
        ["$other"]=>
        string(10) "<optional>"
      }
    }
    [u"var"]=>
    int(42)
  }
  ["static"]=>
  array(1) {
    [u"outer"]=>
    int(25)
  }
  ["parameter"]=>
  array(2) {
    ["$param"]=>
    string(10) "<required>"
    ["$other"]=>
    string(10) "<optional>"
  }
}
object(Closure)#%d (3) {
  ["this"]=>
  object(Test)#%d (3) {
    [u"func1"]=>
    object(Closure)#%d (3) {
      ["this"]=>
      object(Test)#%d (3) {
        [u"func1"]=>
        object(Closure)#%d (3) {
          ["this"]=>
          *RECURSION*
          ["static"]=>
          array(1) {
            [u"outer"]=>
            int(25)
          }
          ["parameter"]=>
          array(2) {
            ["$param"]=>
            string(10) "<required>"
            ["$other"]=>
            string(10) "<optional>"
          }
        }
        [u"var"]=>
        int(42)
        [u"func2"]=>
        object(Closure)#%d (3) {
          ["this"]=>
          *RECURSION*
          ["static"]=>
          array(1) {
            [u"outer"]=>
            &int(25)
          }
          ["parameter"]=>
          array(2) {
            ["$param"]=>
            string(10) "<required>"
            ["$other"]=>
            string(10) "<optional>"
          }
        }
      }
      ["static"]=>
      array(1) {
        [u"outer"]=>
        int(25)
      }
      ["parameter"]=>
      array(2) {
        ["$param"]=>
        string(10) "<required>"
        ["$other"]=>
        string(10) "<optional>"
      }
    }
    [u"var"]=>
    int(42)
    [u"func2"]=>
    object(Closure)#%d (3) {
      ["this"]=>
      object(Test)#%d (3) {
        [u"func1"]=>
        object(Closure)#%d (3) {
          ["this"]=>
          *RECURSION*
          ["static"]=>
          array(1) {
            [u"outer"]=>
            int(25)
          }
          ["parameter"]=>
          array(2) {
            ["$param"]=>
            string(10) "<required>"
            ["$other"]=>
            string(10) "<optional>"
          }
        }
        [u"var"]=>
        int(42)
        [u"func2"]=>
        object(Closure)#%d (3) {
          ["this"]=>
          *RECURSION*
          ["static"]=>
          array(1) {
            [u"outer"]=>
            &int(25)
          }
          ["parameter"]=>
          array(2) {
            ["$param"]=>
            string(10) "<required>"
            ["$other"]=>
            string(10) "<optional>"
          }
        }
      }
      ["static"]=>
      array(1) {
        [u"outer"]=>
        &int(25)
      }
      ["parameter"]=>
      array(2) {
        ["$param"]=>
        string(10) "<required>"
        ["$other"]=>
        string(10) "<optional>"
      }
    }
  }
  ["static"]=>
  array(1) {
    [u"outer"]=>
    &int(25)
  }
  ["parameter"]=>
  array(2) {
    ["$param"]=>
    string(10) "<required>"
    ["$other"]=>
    string(10) "<optional>"
  }
}
object(Closure)#%d (3) {
  ["this"]=>
  NULL
  ["static"]=>
  array(1) {
    [u"outer"]=>
    int(25)
  }
  ["parameter"]=>
  array(2) {
    ["$param"]=>
    string(10) "<required>"
    ["$other"]=>
    string(10) "<optional>"
  }
}
===DONE===
