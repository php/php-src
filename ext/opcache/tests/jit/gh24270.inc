<?php
class Base {
	public static function staticSelf(): Closure {
		return fn() => self::make();
	}
	public function boundSelf(): Closure {
		return fn() => self::make();
	}
	private static function make() {
		return new static();
	}
}
class Mid extends Base {
	protected static function name() {
		return static::class;
	}
}
class Child extends Mid {
	public function boundParent(): Closure {
		return fn() => parent::name();
	}
}
class GrandChild extends Child {}

class ParentA {
	public static function name() {
		return static::class;
	}
}
class ParentB {
	public static function name() {
		return static::class;
	}
}
trait T {
	public static function traitSelf(): Closure {
		return fn() => self::make();
	}
	public function traitParent(): Closure {
		return fn() => parent::name();
	}
	public static function make() {
		return new static();
	}
}
class A extends ParentA {
	use T;
}
class ChildA extends A {}
class B extends ParentB {
	use T;
}
class ChildB extends B {}

$closures = [
	'self from static method' => Child::staticSelf(),
	'bound self' => (new Child)->boundSelf(),
	'bound parent' => (new GrandChild)->boundParent(),
	'trait self A' => ChildA::traitSelf(),
	'trait self B' => ChildB::traitSelf(),
	'trait parent A' => (new ChildA)->traitParent(),
	'trait parent B' => (new ChildB)->traitParent(),
];

$results = [];
for ($i = 0; $i < 100; $i++) {
	foreach ($closures as $name => $closure) {
		$result = $closure();
		$results[$name][is_object($result) ? $result::class : $result] = true;
	}
}
foreach ($results as $name => $classes) {
	echo $name, ': ', implode(', ', array_keys($classes)), "\n";
}
