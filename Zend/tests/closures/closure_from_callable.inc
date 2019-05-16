<?php

function bar($param1)
{
	return $param1;
}


$closure = function($param1) {
	return $param1;
};

function test($fn)
{
	static $count = 0;
	$input = "foo".$count;
	$count++;

	$output = $fn($input);
	return $input === $output;
}

class Foo
{
	public static function publicStaticFunction($param1)
	{
		return $param1;
	}

	private static function privateStaticFunction($param1)
	{
		return $param1;
	}

	protected static function protectedStaticFunction($param1)
	{
		return $param1;
	}

	private function privateInstanceFunc($param1)
	{
		return $param1;
	}

	protected function protectedInstanceFunc($param1)
	{
		return $param1;
	}


	public function publicInstanceFunc($param1)
	{
		return $param1;
	}

	public function closePrivateValid()
	{
		return Closure::fromCallable([$this, 'privateInstanceFunc']);
	}

	public function closePrivateStatic()
	{
		return Closure::fromCallable([__CLASS__, 'privateStaticFunction']);
	}

	public function bar($param1)
	{
		echo "this is bar\n";
	}

	public function getCallable()
	{
		return Closure::fromCallable([$this, 'publicInstanceFunc']);
	}

	public function getSelfPublicInstance()
	{
		return Closure::fromCallable([$this, 'publicInstanceFunc']);
	}

	public function getSelfColonPublicInstanceMethod()
	{
		return Closure::fromCallable('self::publicInstanceFunc');
	}
}



class SubFoo extends Foo {

	public function closePrivateStaticInvalid()
	{
		return Closure::fromCallable([__CLASS__, 'privateStaticFunction']);
	}


	public function closePrivateInvalid()
	{
		return Closure::fromCallable([$this, 'privateInstanceFunc']);
	}

	public function closeProtectedStaticMethod()
	{
		return Closure::fromCallable([__CLASS__, 'protectedStaticFunction']);
	}

	public function closeProtectedValid()
	{
		return Closure::fromCallable([$this, 'protectedInstanceFunc']);
	}

	public function getParentPublicInstanceMethod()
	{
		return Closure::fromCallable('parent::publicInstanceFunc');
	}

	public function getSelfColonParentPublicInstanceMethod()
	{
		return Closure::fromCallable('self::publicInstanceFunc');
	}


	public function getSelfColonParentProtectedInstanceMethod()
	{
		return Closure::fromCallable('self::protectedInstanceFunc');
	}

	public function getSelfColonParentPrivateInstanceMethod()
	{
		return Closure::fromCallable('self::privateInstanceFunc');
	}
}


class MagicCall
{
	public function __call($name, $arguments)
	{
		$info = ['__call'];
		$info[] = $name;
		$info = array_merge($info, $arguments);
		return implode(',', $info);
	}

	public static function __callStatic($name, $arguments)
	{
		$info = ['__callStatic'];
		$info[] = $name;
		$info = array_merge($info, $arguments);
		return implode(',', $info);
	}
}



class PublicInvokable
{
	public function __invoke($param1)
	{
		return $param1;
	}
}


function functionAccessProtected()
{
	$foo = new Foo;

	return Closure::fromCallable([$foo, 'protectedStaticFunction']);
}

function functionAccessPrivate()
{
	$foo = new Foo;

	return Closure::fromCallable([$foo, 'privateStaticFunction']);
}


function functionAccessMethodDoesntExist()
{
	$foo = new Foo;

	return Closure::fromCallable([$foo, 'thisDoesNotExist']);
}

?>
