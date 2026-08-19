--TEST--
GH-9516: (A&B)|D as a param should allow AB or D. Not just A.
--FILE--
<?php

interface A { }
interface B { }
interface D { }

class A_ implements A {}
class B_ implements B {}
class AB_ implements A, B {}
class D_ implements D {}

class T {
    public function method1((A&B)|D $arg): void {}
    public function method2((B&A)|D $arg): void {}
    public function method3(D|(A&B) $arg): void {}
    public function method4(D|(B&A) $arg): void {}
}

$t = new T;

try {
    $t->method1(new A_);
    echo 'Fail', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method1(new B_);
    echo 'Fail', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method1(new AB_);
    echo 'Pass', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method1(new D_);
    echo 'Pass', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

// Lets try in reverse?
try {
    $t->method2(new A_);
    echo 'Fail', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method2(new B_);
    echo 'Fail', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method2(new AB_);
    echo 'Pass', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method2(new D_);
    echo 'Pass', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

/* Single before intersection */
try {
    $t->method3(new A_);
    echo 'Fail', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method3(new B_);
    echo 'Fail', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method3(new AB_);
    echo 'Pass', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method3(new D_);
    echo 'Pass', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

// Lets try in reverse?
try {
    $t->method4(new A_);
    echo 'Fail', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method4(new B_);
    echo 'Fail', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method4(new AB_);
    echo 'Pass', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}

try {
    $t->method4(new D_);
    echo 'Pass', \PHP_EOL;
} catch (\Throwable $throwable) {
    echo $throwable::class, ': ', $throwable->getMessage(), "\n";
}


?>
--EXPECTF--
TypeError: T::method1(): Argument #1 ($arg) must be of type (A&B)|D, A_ given, called in %s on line %d
TypeError: T::method1(): Argument #1 ($arg) must be of type (A&B)|D, B_ given, called in %s on line %d
Pass
Pass
TypeError: T::method2(): Argument #1 ($arg) must be of type (B&A)|D, A_ given, called in %s on line %d
TypeError: T::method2(): Argument #1 ($arg) must be of type (B&A)|D, B_ given, called in %s on line %d
Pass
Pass
TypeError: T::method3(): Argument #1 ($arg) must be of type D|(A&B), A_ given, called in %s on line %d
TypeError: T::method3(): Argument #1 ($arg) must be of type D|(A&B), B_ given, called in %s on line %d
Pass
Pass
TypeError: T::method4(): Argument #1 ($arg) must be of type D|(B&A), A_ given, called in %s on line %d
TypeError: T::method4(): Argument #1 ($arg) must be of type D|(B&A), B_ given, called in %s on line %d
Pass
Pass
