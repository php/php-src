--TEST--
GH-25766: enchant_dict_suggest() crashes with JIT when the result for a word without suggestions is modified
--EXTENSIONS--
enchant
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--FILE--
<?php
function test(EnchantDictionary $dictionary, string $word) {
    $suggestions = enchant_dict_suggest($dictionary, $word);
    $suggestions[] = 1;
    return $suggestions;
}

$broker = enchant_broker_init();
$dictionary = enchant_broker_request_pwl_dict($broker, __DIR__ . "/enchant_broker_request_pwl_dict.pwl");
var_dump(test($dictionary, "zzzzzzzzzzzz"));
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
