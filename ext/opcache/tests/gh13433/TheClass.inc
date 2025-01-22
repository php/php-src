<?php
class TheClass extends ParentClass {
	use TraitA;

	public function test() {
		return self::$non_empty_array;
	}
}
