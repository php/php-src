--TEST--
assert() - basic - test initial values using ini.get and assert_options
--INI--
assert.active = 0
assert.warning = 0
assert.bail = 0
assert.callback = f1
--FILE--
<?php
// Check the initial settings for all assert_options

//Using assert_options;
echo "Initial values: assert_options(ASSERT_ACTIVE) => [".assert_options(ASSERT_ACTIVE)."]\n";
echo "Initial values: assert_options(ASSERT_WARNING) => [".assert_options(ASSERT_WARNING)."]\n";
echo "Initial values: assert_options(ASSERT_BAIL) => [".assert_options(ASSERT_BAIL)."]\n";
echo "Initial values: assert_options(ASSERT_CALLBACK) => [".assert_options(ASSERT_CALLBACK)."]\n";

//Using ini.get;
echo "Initial values: ini.get(\"assert.active\") => [".ini_get("assert.active")."]\n";
echo "Initial values: ini.get(\"assert.warning\") => [".ini_get("assert.warning")."]\n";
echo "Initial values: ini.get(\"assert.bail\") => [".ini_get("assert.bail")."]\n";
echo "Initial values: ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n\n";
?>
--EXPECTF--
Deprecated: PHP Startup: assert.active INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: assert.warning INI setting is deprecated in Unknown on line 0

Deprecated: PHP Startup: assert.callback INI setting is deprecated in Unknown on line 0

Deprecated: Constant ASSERT_ACTIVE is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
Initial values: assert_options(ASSERT_ACTIVE) => [0]

Deprecated: Constant ASSERT_WARNING is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
Initial values: assert_options(ASSERT_WARNING) => [0]

Deprecated: Constant ASSERT_BAIL is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
Initial values: assert_options(ASSERT_BAIL) => [0]

Deprecated: Constant ASSERT_CALLBACK is deprecated in %s on line %d

Deprecated: Function assert_options() is deprecated in %s on line %d
Initial values: assert_options(ASSERT_CALLBACK) => [f1]
Initial values: ini.get("assert.active") => [0]
Initial values: ini.get("assert.warning") => [0]
Initial values: ini.get("assert.bail") => [0]
Initial values: ini.get("assert.callback") => [f1]
