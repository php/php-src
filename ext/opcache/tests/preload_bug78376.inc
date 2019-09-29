<?php
const CNST = 'aaaa';
class A {
	public static $a = CNST;
}
$a = \A::$a;
