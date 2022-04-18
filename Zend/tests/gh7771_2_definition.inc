<?php

class Foo {
	public const BAR = 
		self::BAZ
		+ NonExistent::CLASS_CONSTANT;
	public const BAZ = 42;
}
