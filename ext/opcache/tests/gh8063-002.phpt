--TEST--
Bug GH-8063 (Opcache breaks autoloading after E_COMPILE_ERROR) 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.record_warnings=1
--EXTENSIONS--
opcache
--FILE--
<?php

spl_autoload_register(function ($class) {
    printf("Autoloading %s\n", $class);
    include __DIR__.DIRECTORY_SEPARATOR.'gh8063'.DIRECTORY_SEPARATOR.$class.'.inc';
});

register_shutdown_function(function () {
    new Bar();
    new Baz();
    print "Finished\n";
});

new BadClass();
?>
--EXPECTF--
Autoloading BadClass
Autoloading Foo

Fatal error: Declaration of BadClass::dummy() must be compatible with Foo::dummy(): void in %sBadClass.inc on line 5
Autoloading Bar
Autoloading Baz
Finished
