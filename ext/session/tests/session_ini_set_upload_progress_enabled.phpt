--TEST--
ini_set() for session.* is not allowed when session.upload_progress.enabled is On
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.upload_progress.enabled=1
session.save_path=
session.name=NOTPHPSESSID
session.save_handler=files
session.serialize_handler=php_serialize
session.cookie_lifetime=42
session.gc_probability=2
session.cookie_path=/startup
session.cookie_secure=1
session.sid_length=40
session.sid_bits_per_character=5
--FILE--
<?php
/* The INI section above is applied at the startup stage, so these values must
 * have been applied even though session.upload_progress.enabled is On. */
echo "*** Settings applied at startup ***\n";
foreach ([
    "session.name",
    "session.serialize_handler",
    "session.cookie_lifetime",
    "session.gc_probability",
    "session.cookie_path",
    "session.cookie_secure",
    "session.sid_length",
    "session.sid_bits_per_character",
] as $name) {
    printf("%s=%s\n", $name, var_export(ini_get($name), true));
}

echo "\n*** ini_set() is rejected at runtime ***\n";

function try_ini_set($name, $value) {
    try {
        var_dump(ini_set($name, $value));
    } catch (\Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}

/* One entry per ini modify handler guarded by the check. */
try_ini_set("session.save_path", "/tmp");
try_ini_set("session.name", "PHPSESSID");
try_ini_set("session.save_handler", "user");
try_ini_set("session.serialize_handler", "php");
try_ini_set("session.cookie_lifetime", "1");
try_ini_set("session.gc_probability", "1");
try_ini_set("session.cookie_path", "/runtime");
try_ini_set("session.cookie_secure", "0");
try_ini_set("session.sid_length", "32");
try_ini_set("session.sid_bits_per_character", "4");

echo "\n*** ini_restore() is rejected at runtime ***\n";
try {
    var_dump(ini_restore("session.name"));
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
var_dump(ini_get("session.name"));

echo "\n*** Values are unchanged ***\n";
foreach ([
    "session.name",
    "session.serialize_handler",
    "session.cookie_lifetime",
    "session.gc_probability",
    "session.cookie_path",
    "session.cookie_secure",
    "session.sid_length",
    "session.sid_bits_per_character",
] as $name) {
    printf("%s=%s\n", $name, var_export(ini_get($name), true));
}

echo "\n*** Non-session ini settings are unaffected ***\n";
var_dump(ini_set("precision", "10"));
var_dump(ini_get("precision"));

echo "Done\n";
?>
--EXPECT--
*** Settings applied at startup ***
session.name='NOTPHPSESSID'
session.serialize_handler='php_serialize'
session.cookie_lifetime='42'
session.gc_probability='2'
session.cookie_path='/startup'
session.cookie_secure='1'
session.sid_length='40'
session.sid_bits_per_character='5'

*** ini_set() is rejected at runtime ***
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On

*** ini_restore() is rejected at runtime ***
Error: Session ini settings cannot be changed at runtime when session.upload_progress.enabled is On
string(12) "NOTPHPSESSID"

*** Values are unchanged ***
session.name='NOTPHPSESSID'
session.serialize_handler='php_serialize'
session.cookie_lifetime='42'
session.gc_probability='2'
session.cookie_path='/startup'
session.cookie_secure='1'
session.sid_length='40'
session.sid_bits_per_character='5'

*** Non-session ini settings are unaffected ***
string(2) "14"
string(2) "10"
Done
