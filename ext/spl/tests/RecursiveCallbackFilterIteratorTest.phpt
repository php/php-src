--TEST--
RecursiveCallbackFilterIterator
--FILE--
<?php

class A {
    function test($value, $key, $inner) {
        return test($value, $key, $inner);
    }
}

class B {
    static function test($value, $key, $inner) {
        return test($value, $key, $inner);
    }
}

function test($value, $key, $inner) {
	if ($inner->hasChildren()) {
		return true;
	}
	printf("%s / %s / %d / %d\n"
		, print_r($value, true)
		, $key
		, $value == $inner->current()
		, $key == $inner->key()
	);
	return $value === 1 || $value === 4;
}

$tests = array(
    'instance method'    => function() { return array(new A, 'test'); },
    'static method'      => function() { return array('B', 'test'); },
    'static method (2)'  => function() { return 'B::test'; },
    'function'           => function() { return 'test'; },
    'anonymous function' => function() { return function($value, $key, $inner) { return test($value, $key, $inner); }; },
);

foreach($tests as $name => $test) {

    $callback = $test();
    $it = new RecursiveArrayIterator(array(1, array(2, 3), array(4, 5)));
    $it = new RecursiveCallbackFilterIterator($it, $callback);
	$it = new RecursiveIteratorIterator($it);

    echo " = $name =\n";

    foreach($it as $value) {
        echo "=> $value\n";
    }

	// same test, with no reference to callback

    $it = new RecursiveArrayIterator(array(1, array(2, 3), array(4, 5)));
    $it = new RecursiveCallbackFilterIterator($it, $test());
	$it = new RecursiveIteratorIterator($it);
    unset($callback);

    foreach($it as $value) {
        echo "=> $value\n";
    }
}
--EXPECT--
= instance method =
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
 = static method =
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
 = static method (2) =
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
 = function =
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
 = anonymous function =
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
1 / 0 / 1 / 1
=> 1
2 / 0 / 1 / 1
3 / 1 / 1 / 1
4 / 0 / 1 / 1
=> 4
5 / 1 / 1 / 1
