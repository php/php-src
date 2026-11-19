--TEST--
set_include_path() result is not freed while the old include_path is still referenced under JIT
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--ENV--
INC_PATH=inc_path_a
--FILE--
<?php
$path = getenv("INC_PATH");
set_include_path($path);
set_include_path("inc_path_b");
echo "argument: ";
var_dump($path);

set_include_path(getenv("INC_PATH"));
$old = get_include_path();
set_include_path("inc_path_b");
echo "old include_path: ";
var_dump($old);
echo "new include_path: ";
var_dump(get_include_path());
?>
--EXPECT--
argument: string(10) "inc_path_a"
old include_path: string(10) "inc_path_a"
new include_path: string(10) "inc_path_b"
