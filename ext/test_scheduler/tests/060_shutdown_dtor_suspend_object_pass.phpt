--TEST--
Object destructor suspends during the object-store shutdown pass: the iterator carries the pass on
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

class Holder {
    public static array $keep = [];
}

class Suspends {
    public function __destruct() {
        echo "Suspends::dtor enter\n";
        TestScheduler\await(TestScheduler\spawn(function () {
            echo "spawned body\n";
            return 1;
        }));
        echo "Suspends::dtor leave\n";
    }
}

class Plain {
    public function __construct(private string $name) {}
    public function __destruct() {
        echo "Plain::dtor {$this->name}\n";
    }
}

Holder::$keep[] = new Suspends();
Holder::$keep[] = new Plain("b");
Holder::$keep[] = new Plain("c");

echo "==DONE==\n";
?>
--EXPECT--
==DONE==
Suspends::dtor enter
spawned body
Plain::dtor b
Plain::dtor c
Suspends::dtor leave
