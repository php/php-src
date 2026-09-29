--TEST--
unserialize allowed_classes_callback
--FILE--
<?php

class TestClass {
}

class AnotherTestClass {
}

// Basic usage
$serialized = serialize(
    [
        new TestClass(),
        new TestClass(),
    ]
);

$dummy = unserialize(
    $serialized,
    [
        'allowed_classes_callback' => function ($className) {
            var_dump($className);
            return true;
        }
    ]
);

var_dump($dummy);


// allowed_classes takes precedent to allowed_classes_callback, which is never called in this case
$dummy = unserialize(
    $serialized,
    [
        'allowed_classes' => ['TestClass'],
        'allowed_classes_callback' => function ($className) {
            var_dump($className);
            return true;
        }
    ]
);

var_dump($dummy);


// unserialize() blocked class
$dummy = unserialize(
    $serialized,
    [
        'allowed_classes_callback' => function ($className) {
            return false;
        }
    ]
);

var_dump($dummy);

// Nested unserialize() one is allowed, the second blocked
$flip = false;
$dummy = unserialize(
    $serialized,
    [
        'allowed_classes_callback' => function ($className) use (&$flip) {
            $serialized = serialize(
                [
                    new AnotherTestClass(),
                ]
            );

            $dummy = unserialize(
                $serialized,
                [
                    'allowed_classes_callback' => function ($className) use (&$flip) {
                        echo 'Nested: ';
                        var_dump($className);
                        $flip = !$flip;
                        return $flip;
                    }
                ]
            );

            echo 'Nested: ';
            var_dump($dummy);
            return true;
        }
    ]
);

var_dump($dummy);


// throw from inside the callback
try {
    $dummy = unserialize(
        $serialized,
        [
            'allowed_classes_callback' => function ($className) {
                throw new RuntimeException('Better not unserialize this');
            }
        ]
    );
} catch (RuntimeException $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(9) "TestClass"
string(9) "TestClass"
array(2) {
  [0]=>
  object(TestClass)#1 (0) {
  }
  [1]=>
  object(TestClass)#3 (0) {
  }
}
array(2) {
  [0]=>
  object(TestClass)#4 (0) {
  }
  [1]=>
  object(TestClass)#5 (0) {
  }
}
array(2) {
  [0]=>
  object(__PHP_Incomplete_Class)#1 (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(9) "TestClass"
  }
  [1]=>
  object(__PHP_Incomplete_Class)#2 (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(9) "TestClass"
  }
}
Nested: string(16) "AnotherTestClass"
Nested: array(1) {
  [0]=>
  object(AnotherTestClass)#3 (0) {
  }
}
Nested: string(16) "AnotherTestClass"
Nested: array(1) {
  [0]=>
  object(__PHP_Incomplete_Class)#6 (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(16) "AnotherTestClass"
  }
}
array(2) {
  [0]=>
  object(TestClass)#3 (0) {
  }
  [1]=>
  object(TestClass)#6 (0) {
  }
}
string(27) "Better not unserialize this"