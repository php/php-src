--TEST--
Test zval ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';
$functions = [
    'zend_zval',
    'zend_zval_or_null',
    'zend_zval_separate',
    'zend_zval_slow_zpp',
    'zend_zval_or_null_slow_zpp',
    'zend_zval_deref_separate',
];

foreach ($functions as $i => $function) {
    echo "Using $function:\n\n";

    foreach ($types as $name => $type) {
        printf("  %-16s ", "$name:");

        try {
            $result = $function($type);

            $result === []
                ? printf("array(0) {}\n")
                : var_dump($result);

        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }

    if ($i !== array_key_last($functions)) {
        echo "\n";
    }
}

?>
--EXPECTF--
Using zend_zval:

  null:            NULL
  false:           bool(false)
  true:            bool(true)
  42:              int(42)
  73.5:            float(73.5)
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  object(stdClass)#%d (%d) {
}
  new S():         object(S)#%d (%d) {
}
  STDOUT:          resource(%d) of type (stream)

Using zend_zval_or_null:

  null:            NULL
  false:           bool(false)
  true:            bool(true)
  42:              int(42)
  73.5:            float(73.5)
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  object(stdClass)#%d (%d) {
}
  new S():         object(S)#%d (%d) {
}
  STDOUT:          resource(%d) of type (stream)

Using zend_zval_separate:

  null:            NULL
  false:           bool(false)
  true:            bool(true)
  42:              int(42)
  73.5:            float(73.5)
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  object(stdClass)#%d (%d) {
}
  new S():         object(S)#%d (%d) {
}
  STDOUT:          resource(%d) of type (stream)

Using zend_zval_slow_zpp:

  null:            NULL
  false:           bool(false)
  true:            bool(true)
  42:              int(42)
  73.5:            float(73.5)
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  object(stdClass)#%d (%d) {
}
  new S():         object(S)#%d (%d) {
}
  STDOUT:          resource(%d) of type (stream)

Using zend_zval_or_null_slow_zpp:

  null:            NULL
  false:           bool(false)
  true:            bool(true)
  42:              int(42)
  73.5:            float(73.5)
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  object(stdClass)#%d (%d) {
}
  new S():         object(S)#%d (%d) {
}
  STDOUT:          resource(%d) of type (stream)

Using zend_zval_deref_separate:

  null:            NULL
  false:           bool(false)
  true:            bool(true)
  42:              int(42)
  73.5:            float(73.5)
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  object(stdClass)#%d (%d) {
}
  new S():         object(S)#%d (%d) {
}
  STDOUT:          resource(%d) of type (stream)
