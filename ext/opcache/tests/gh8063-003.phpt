--TEST--
Bug GH-8063 (Opcache breaks autoloading after E_COMPILE_ERROR) 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.record_warnings=0
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

new BadClass2();
--EXPECTF--
Autoloading BadClass2

Fatal error: Declaration of BadClass2::dummy() must be compatible with Foo2::dummy(): void in %sBadClass2.inc on line %d
Autoloading Bar
Autoloading Baz
Finished
