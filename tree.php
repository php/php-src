<?php
$tokens = token_get_all('<?php
function noOp(): void {}');

var_export($tokens);
