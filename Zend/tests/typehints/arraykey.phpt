--TEST--
Arraykey parameter, return, and property type hints
--FILE--
<?php

abstract class ArrayKeyFoo {
  public arraykey $baz;

  public function __construct() {
    $this->baz = 4;
  }

  public function bar(string $bar): arraykey {
    return $bar;
  }
  abstract public function baz(string ...$args): void;
}

class ArrayKeyBar extends ArrayKeyFoo {
  public arraykey $bar = 'foo';

  public function bar(arraykey $bar): string {
    return (string) parent::bar((string) $bar);
  }
  public function baz(arraykey ...$args): void {}
}

$bar = new ArrayKeyBar();
$bar->baz = 'f';
$bar->baz = 4;

try {
  $bar->baz = false;
} catch(\TypeError $e) {
  echo "OK\n";
}

try {
  $bar->baz = 4.3;
} catch(\TypeError $e) {
  echo "OK\n";
}

$bar->bar('f');
$bar->bar(4);

try {
  $bar->baz(false);
} catch(\TypeError $e) {
  echo "OK\n";
}

try {
  $bar->baz(4.3);
} catch(\TypeError $e) {
  echo "OK\n";
}

function _arraykey_nullable(?arraykey $b): void {}

_arraykey_nullable(44);
_arraykey_nullable('d');
_arraykey_nullable(null);

try {
  _arraykey_nullable(4.3);
} catch(\TypeError $e) {
  echo "OK\n";
}


function _arraykey_nullable_return_null(): ?arraykey { return null; }
function _arraykey_nullable_return_string(): ?arraykey { return 'foo'; }
function _arraykey_nullable_return_int(): ?arraykey { return 'bar'; }
function _arraykey_nullable_return_float(): ?arraykey { return 3.2; }

_arraykey_nullable_return_null();
_arraykey_nullable_return_string();
_arraykey_nullable_return_int();

try {
  _arraykey_nullable_return_float();
} catch(\TypeError $e) {
  echo "OK\n";
}

function _arraykey_return_string(): arraykey { return ''; }
function _arraykey_return_int(): arraykey { return 2; }
function _arraykey_return_float(): arraykey { return 3.3; }

_arraykey_return_string();
_arraykey_return_int();

try {
  _arraykey_return_float();
} catch(\TypeError $e) {
  echo "OK\n";
}

--EXPECT--
OK
OK
OK
OK
OK
OK
OK
