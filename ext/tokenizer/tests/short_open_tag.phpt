--TEST--
token_get_all() TOKEN_*_SHORT_OPEN_TAG flag
--EXTENSIONS--
tokenizer
--FILE--
<?php
// This can be overridden for individual calls to token_get_all
echo json_encode(token_get_all('<?xml', TOKEN_DISABLE_SHORT_OPEN_TAG)), "\n";
echo json_encode(token_get_all('<?xml', TOKEN_ENABLE_SHORT_OPEN_TAG)), "\n";
// When setting both flags, TOKEN_DISABLE_SHORT_OPEN_TAG takes precedence
echo json_encode(token_get_all('<?xml', TOKEN_DISABLE_SHORT_OPEN_TAG|TOKEN_ENABLE_SHORT_OPEN_TAG)), "\n";
echo json_encode(PhpToken::tokenize('<?xml', TOKEN_DISABLE_SHORT_OPEN_TAG)), "\n";
echo json_encode(PhpToken::tokenize('<?xml', TOKEN_ENABLE_SHORT_OPEN_TAG)), "\n";
?>
--EXPECTF--
[[%d,"<?xml",1]]
[[%d,"<?",1],[%d,"xml",1]]
[[%d,"<?xml",1]]
[{"id":%d,"text":"<?xml","line":1,"pos":0}]
[{"id":%d,"text":"<?","line":1,"pos":0},{"id":%d,"text":"xml","line":1,"pos":2}]
