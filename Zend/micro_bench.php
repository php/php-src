<?php

function hallo() {
}

function simpleucall($n) {
  for ($i = 0; $i < $n; $i++)
    hallo();
}

function simpleudcall($n) {
  for ($i = 0; $i < $n; $i++)
    hallo2();
}

function hallo2() {
}

function simpleicall($n) {
  for ($i = 0; $i < $n; $i++)
    func_num_args();
}

class Foo {
	static $a = 0;
	public $b = 0;
	const TEST = 0;

	static function read_static($n) {
		for ($i = 0; $i < $n; ++$i) {
			$x = self::$a;
		}
	}

	static function write_static($n) {
		for ($i = 0; $i < $n; ++$i) {
			self::$a = 0;
		}
	}

	static function isset_static($n) {
		for ($i = 0; $i < $n; ++$i) {
			$x = isset(self::$a);
		}
	}

	static function empty_static($n) {
		for ($i = 0; $i < $n; ++$i) {
			$x = empty(self::$a);
		}
	}

	static function f() {
	}

	static function call_static($n) {
		for ($i = 0; $i < $n; ++$i) {
			self::f();
		}
	}

	function read_prop($n) {
		for ($i = 0; $i < $n; ++$i) {
			$x = $this->b;
		}
	}

	function write_prop($n) {
		for ($i = 0; $i < $n; ++$i) {
			$this->b = 0;
		}
	}

	function assign_add_prop($n) {
		for ($i = 0; $i < $n; ++$i) {
			$this->b += 2;
		}
	}

	function pre_inc_prop($n) {
		for ($i = 0; $i < $n; ++$i) {
			++$this->b;
		}
	}

	function pre_dec_prop($n) {
		for ($i = 0; $i < $n; ++$i) {
			--$this->b;
		}
	}

	function post_inc_prop($n) {
		for ($i = 0; $i < $n; ++$i) {
			$this->b++;
		}
	}

	function post_dec_prop($n) {
		for ($i = 0; $i < $n; ++$i) {
			$this->b--;
		}
	}

	function isset_prop($n) {
		for ($i = 0; $i < $n; ++$i) {
			$x = isset($this->b);
		}
	}

	function empty_prop($n) {
		for ($i = 0; $i < $n; ++$i) {
			$x = empty($this->b);
		}
	}

	function g() {
	}

	function call($n) {
		for ($i = 0; $i < $n; ++$i) {
			$this->g();
		}
	}

	function read_const($n) {
		for ($i = 0; $i < $n; ++$i) {
			$x = $this::TEST;
		}
	}

}

function read_static($n) {
	for ($i = 0; $i < $n; ++$i) {
		$x = Foo::$a;
	}
}

function write_static($n) {
	for ($i = 0; $i < $n; ++$i) {
		Foo::$a = 0;
	}
}

function isset_static($n) {
	for ($i = 0; $i < $n; ++$i) {
		$x = isset(Foo::$a);
	}
}

function empty_static($n) {
	for ($i = 0; $i < $n; ++$i) {
		$x = empty(Foo::$a);
	}
}

function call_static($n) {
	for ($i = 0; $i < $n; ++$i) {
		Foo::f();
	}
}

function create_object($n) {
	for ($i = 0; $i < $n; ++$i) {
		$x = new Foo();
	}
}

define('TEST', null);

function read_const($n) {
	for ($i = 0; $i < $n; ++$i) {
		$x = TEST;
	}
}

function read_auto_global($n) {
	for ($i = 0; $i < $n; ++$i) {
		$x = $_GET;
	}
}

$g_var = 0;

function read_global_var($n) {
	for ($i = 0; $i < $n; ++$i) {
		$x = $GLOBALS['g_var'];
	}
}

function read_hash($n) {
	$hash = array('test' => 0);
	for ($i = 0; $i < $n; ++$i) {
		$x = $hash['test'];
	}
}

function read_str_offset($n) {
	$str = "test";
	for ($i = 0; $i < $n; ++$i) {
		$x = $str[1];
	}
}

function issetor($n) {
	$val = array(0,1,2,3,4,5,6,7,8,9);
	for ($i = 0; $i < $n; ++$i) {
		$x = $val ?: null;
	}
}

function issetor2($n) {
	$f = false; $j = 0;
	for ($i = 0; $i < $n; ++$i) {
		$x = $f ?: $j + 1;
	}
}

function ternary($n) {
	$val = array(0,1,2,3,4,5,6,7,8,9);
	$f = false;
	for ($i = 0; $i < $n; ++$i) {
		$x = $f ? null : $val;
	}
}

function ternary2($n) {
	$f = false; $j = 0;
	for ($i = 0; $i < $n; ++$i) {
		$x = $f ? $f : $j + 1;
	}
}

/*****/

function empty_loop($n) {
	for ($i = 0; $i < $n; ++$i) {
	}
}

function gethrtime()
{
  $hrtime = hrtime();
  return (($hrtime[0]*1000000000 + $hrtime[1]) / 1000000000);
}

function start_test()
{
  ob_start();
  return gethrtime();
}

function end_test($start, $name, $overhead = null)
{
  global $total;
  global $last_time;
  $end = gethrtime();
  ob_end_clean();
  $last_time = $end-$start;
  $total += $last_time;
  $num = number_format($last_time,3);
  $pad = str_repeat(" ", 24-strlen($name)-strlen($num));
  if (is_null($overhead)) {
    echo $name.$pad.$num."\n";
  } else {
    $num2 = number_format($last_time - $overhead,3);
    echo $name.$pad.$num."    ".$num2."\n";
  }
  ob_start();
  return gethrtime();
}

function total()
{
  global $total;
  $pad = str_repeat("-", 24);
  echo $pad."\n";
  $num = number_format($total,3);
  $pad = str_repeat(" ", 24-strlen("Total")-strlen($num));
  echo "Total".$pad.$num."\n";
}

const N = 5000000;

$t0 = $t = start_test();
empty_loop(N);
$t = end_test($t, 'empty_loop');
$overhead = $last_time;
simpleucall(N);
$t = end_test($t, 'func()', $overhead);
simpleudcall(N);
$t = end_test($t, 'undef_func()', $overhead);
simpleicall(N);
$t = end_test($t, 'int_func()', $overhead);
Foo::read_static(N);
$t = end_test($t, '$x = self::$x', $overhead);
Foo::write_static(N);
$t = end_test($t, 'self::$x = 0', $overhead);
Foo::isset_static(N);
$t = end_test($t, 'isset(self::$x)', $overhead);
Foo::empty_static(N);
$t = end_test($t, 'empty(self::$x)', $overhead);
read_static(N);
$t = end_test($t, '$x = Foo::$x', $overhead);
write_static(N);
$t = end_test($t, 'Foo::$x = 0', $overhead);
isset_static(N);
$t = end_test($t, 'isset(Foo::$x)', $overhead);
empty_static(N);
$t = end_test($t, 'empty(Foo::$x)', $overhead);
Foo::call_static(N);
$t = end_test($t, 'self::f()', $overhead);
call_static(N);
$t = end_test($t, 'Foo::f()', $overhead);
$x = new Foo();
$x->read_prop(N);
$t = end_test($t, '$x = $this->x', $overhead);
$x->write_prop(N);
$t = end_test($t, '$this->x = 0', $overhead);
$x->assign_add_prop(N);
$t = end_test($t, '$this->x += 2', $overhead);
$x->pre_inc_prop(N);
$t = end_test($t, '++$this->x', $overhead);
$x->pre_dec_prop(N);
$t = end_test($t, '--$this->x', $overhead);
$x->post_inc_prop(N);
$t = end_test($t, '$this->x++', $overhead);
$x->post_dec_prop(N);
$t = end_test($t, '$this->x--', $overhead);
$x->isset_prop(N);
$t = end_test($t, 'isset($this->x)', $overhead);
$x->empty_prop(N);
$t = end_test($t, 'empty($this->x)', $overhead);
$x->call(N);
$t = end_test($t, '$this->f()', $overhead);
$x->read_const(N);
$t = end_test($t, '$x = Foo::TEST', $overhead);
create_object(N);
$t = end_test($t, 'new Foo()', $overhead);
read_const(N);
$t = end_test($t, '$x = TEST', $overhead);
read_auto_global(N);
$t = end_test($t, '$x = $_GET', $overhead);
read_global_var(N);
$t = end_test($t, '$x = $GLOBALS[\'v\']', $overhead);
read_hash(N);
$t = end_test($t, '$x = $hash[\'v\']', $overhead);
read_str_offset(N);
$t = end_test($t, '$x = $str[0]', $overhead);
issetor(N);
$t = end_test($t, '$x = $a ?: null', $overhead);
issetor2(N);
$t = end_test($t, '$x = $f ?: tmp', $overhead);
ternary(N);
$t = end_test($t, '$x = $f ? $f : $a', $overhead);
ternary2(N);
$t = end_test($t, '$x = $f ? $f : tmp', $overhead);
total($t0, "Total");
