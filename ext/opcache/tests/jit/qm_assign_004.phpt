--TEST--
JIT QM_ASSIGN: 004 missing type store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php

function getPropertyScopes($props, $flags): array
{
    $propertyScopes = [];
    foreach ($props as $prop) {
        $tmp = 'x'.$flags;
        $propertyScopes[] = $propertyScopes[] = ($flags & 1 ? false : null) . '';
    }

    return $propertyScopes;
}

var_dump(getPropertyScopes(['a'], 0));
var_dump(getPropertyScopes(['a'], 0));
?>
DONE
--EXPECT--
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
DONE
