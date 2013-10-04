--TEST--
Basic keyword support
--FILE--
<?php

class return {
	const finally = 1;

	public $catch = 1;
}

class yield {
	public function __toString() {
		return 1;
	}

	public static function default () {
	}

	public function namespace () {
	}
}

yield::default();

switch (return::finally) {
	case (new return)->catch:
	case new yield:
		echo "O";
	default:
		goto try;
}
echo "goto T_TRY failed";
try:

(new yield)->namespace();

interface global {}

class abstract extends namespace\yield implements \global {
	public function __construct (array $array, callable $call) {
	}
}

if (new abstract(["array_arg"], function () {})) {
	echo "k";

	// Test if it doesn't throw a parse error. fatal error is normal.
	// inner_keyword
	interface function extends const, break, continue, goto, echo, throw, if, do, for, foreach, declare, instanceof, as, switch, print, class, interface, while, trait, extends, implements, new, clone, var, eval, include, require, use, insteadof, isset, empty, abstract, final, private, protected, public, unset, list, array, callable, or, and, xor {}

	// keyword
	$obj->elseif->endif->else->endwhile->endfor->endforeach->enddeclare->endswitch->case->exit;
}

?>
--EXPECTF--
Ok
Fatal error: %s in %s on line %d
