--TEST--
#[\DelayedTargetValidation] with preloaded attribute with errors
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/opcache_validator_errors.inc
opcache.file_cache_only=0
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
$r = new ReflectionClass('DemoTrait');
echo $r . "\n";
$attributes = $r->getAttributes();
var_dump($attributes);
try {
	$attributes[1]->newInstance();
} catch (Error $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
Trait [ <user> trait DemoTrait ] {
  @@ %s %d-%d

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}

array(2) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(23) "DelayedTargetValidation"
  }
  [1]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(22) "AllowDynamicProperties"
  }
}
Error: Cannot apply #[\AllowDynamicProperties] to trait DemoTrait
