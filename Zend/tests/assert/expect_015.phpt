--TEST--
AST pretty-peinter
--INI--
zend.assertions=1
assert.exception=0
--FILE--
<?php
assert(0 && ($a = function () {
	global $a, $$b;
	static $c, $d = 0;
	unset($e);
	$x = isset($a) && !empty($b) || eval($c);
	$x = $a ? $b : $c;
	$x = $a ?: $c;
	$x = $a ?? $b;
	list($a, $b, $c) = [1, 2=>'x', 'z'=>'c'];
	@foo();
	$y = clone $x;
	yield 1 => 2;
	yield from $x;
}));

assert(0 && ($a = function &(array &$a, ?X $b = null) use ($c,&$d) : ?X {
	abstract class A extends B implements C, D {
		const X = 12;
		const Y = self::X, Z = "aaa";

		public $a = 1, $b;
		protected $c;
		static private $d = null;

		abstract function foo();

		static private function f1() {
			for ($i = 0, $j = 100; $i < $j; $i++, --$j) {
				$s[$i] = $a[$j];
			}
			foreach ($a as $key => &$val) {
				print "$key => $val\n";
			}
			while ($s[$i]) {
				$i++;
			}
			do {
				$i--;
			} while ($s[$i]);
			$x = foo($a + 1, 4, ...[1,2,3]);
			$x = ${$a . "_1"}();
			$x = A::foo();
			$x = ${$a . "_1"}::foo();
			$x = A::${$a . "_1"}();
			$x = $x->foo();
			$x = ${$a . "_1"}->foo();
			$x = $x->{$a . "_1"}();
			$x->a = C::C;
			${$a . "_1"}->a = ${$a . "_1"}::C;
			$x->{a . "_1"} = C::C;
			$x = C::$z;
			$x = ${$a . "_1"}::$z;
			$x = C::${$z . "_1"};
		}
	}
}));

assert(0 && ($a = function &(array &$a, X $b = null) use ($c,&$d) : X {
	final class A {
		final protected function f2() {
			if (!$x) {
				return 0;
			}
			if ($x == 1) {
				return 1;
			} else if ($x == 2) {
				return 2;
			} else if ($x == 3) {
				return 3;
			} else {
				if ($x == 9) {
					return 9;
				}
L0:
                do {
					switch ($x) {
						case 4: break;
						case 5: continue;
						case 6: break 2;
						case 7: continue 2;
						case 8: goto L0;
						default: return;
					}
				} while (0);
			}
		}
	}
}));

assert(0 && ($a = function &(?array &$a, X $b = null) use ($c,&$d) : X {
	class A {
		use T1, T2 {
			T1::foo insteadof foo;
			T2::foo as bar;
			baz as public;
			ops as protected x;
		}
		use T3;
	}
}));

assert(0 && ($a = function &(array &...$a) {
	declare(A=1,B=2);
	try {
		$i++;
	} catch (MyException $e) {
		echo 1;
	} catch (Exception $e) {
		echo 2;
	} finally {
		echo 3;
	}
}));

assert(0 && ($a = function () {
	declare(C=1) { echo 1; }
	$x = '\'"`$a';
	$x = "'\"`$a";
	$x = `'"\`$a`;
	$x = "{$a}b";
	$x = "${a}b";
	$x = " {$foo->bar} ${$foo->bar} ";
	$x = " ${'---'} ";
	foo();
	\foo();
	namespace\foo();
	$x = foo;
	$x = \foo;
	$x = namespace\foo;
	$x = new foo();
	$x = new \foo();
	$x = new namespace\foo();
    if ($a) {
    } elseif ($b) {
    }
    if ($a); else;
}));

?>
--EXPECTF--
Warning: assert(): assert(0 && ($a = function () {
    global $a;
    global $$b;
    static $c;
    static $d = 0;
    unset($e);
    $x = isset($a) && !empty($b) || eval($c);
    $x = $a ? $b : $c;
    $x = $a ?: $c;
    $x = $a ?? $b;
    [$a, $b, $c] = [1, 2 => 'x', 'z' => 'c'];
    @foo();
    $y = clone $x;
    yield 1 => 2;
    yield from $x;
})) failed in %sexpect_015.php on line %d

Warning: assert(): assert(0 && ($a = function &(array &$a, ?X $b = null) use($c, &$d): ?X {
    abstract class A extends B implements C, D {
        const X = 12;
        const Y = self::X, Z = 'aaa';
        public $a = 1, $b;
        protected $c;
        private static $d = null;
        public abstract function foo();

        private static function f1() {
            for ($i = 0, $j = 100; $i < $j; $i++, --$j) {
                $s[$i] = $a[$j];
            }
            foreach ($a as $key => &$val) {
                print "$key => $val\n";
            }
            while ($s[$i]) {
                $i++;
            }
            do {
                $i--;
            } while ($s[$i]);
            $x = foo($a + 1, 4, ...[1, 2, 3]);
            $x = ${$a . '_1'}();
            $x = A::foo();
            $x = ${$a . '_1'}::foo();
            $x = A::${$a . '_1'}();
            $x = $x->foo();
            $x = ${$a . '_1'}->foo();
            $x = $x->{$a . '_1'}();
            $x->a = C::C;
            ${$a . '_1'}->a = ${$a . '_1'}::C;
            $x->{a . '_1'} = C::C;
            $x = C::$z;
            $x = ${$a . '_1'}::$z;
            $x = C::${$z . '_1'};
        }

    }

})) failed in %sexpect_015.php on line %d

Warning: assert(): assert(0 && ($a = function &(array &$a, X $b = null) use($c, &$d): X {
    final class A {
        protected final function f2() {
            if (!$x) {
                return 0;
            }
            if ($x == 1) {
                return 1;
            } else if ($x == 2) {
                return 2;
            } else if ($x == 3) {
                return 3;
            } else {
                if ($x == 9) {
                    return 9;
                }
                L0:
                do {
                    switch ($x) {
                        case 4:
                            break;
                        case 5:
                            continue;
                        case 6:
                            break 2;
                        case 7:
                            continue 2;
                        case 8:
                            goto L0;
                        default:
                            return;
                    }
                } while (0);
            }
        }

    }

})) failed in %sexpect_015.php on line %d

Warning: assert(): assert(0 && ($a = function &(?array &$a, X $b = null) use($c, &$d): X {
    class A {
        use T1, T2 {
            T1::foo insteadof foo;
            T2::foo as bar;
            baz as public;
            ops as protected x;
        }
        use T3;
    }

})) failed in %sexpect_015.php on line %d

Warning: assert(): assert(0 && ($a = function &(array &...$a) {
    declare(A = 1, B = 2);
    try {
        $i++;
    } catch (MyException $e) {
        echo 1;
    } catch (Exception $e) {
        echo 2;
    } finally {
        echo 3;
    }
})) failed in %sexpect_015.php on line %d

Warning: assert(): assert(0 && ($a = function () {
    declare(C = 1) {
        echo 1;
    }
    $x = '\'"`$a';
    $x = "'\"`$a";
    $x = `'"\`$a`;
    $x = "{$a}b";
    $x = "{$a}b";
    $x = " {$foo->bar} {${$foo->bar}} ";
    $x = " ${---} ";
    foo();
    \foo();
    namespace\foo();
    $x = foo;
    $x = \foo;
    $x = namespace\foo;
    $x = new foo();
    $x = new \foo();
    $x = new namespace\foo();
    if ($a) {
    } elseif ($b) {
    }
    if ($a) {
    } else {
    }
})) failed in %sexpect_015.php on line %d
