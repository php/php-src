--TEST--
Test readonly tidyNode properties
--EXTENSIONS--
tidy
--FILE--
<?php

$tidy = tidy_parse_string("<HTML><BODY BGCOLOR=#FFFFFF ALINK=#000000></BODY></HTML>");
$node = $tidy->body();

try {
    $node->value = "";
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $node->name = "";
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $node->type = 1;
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $node->line = 1;
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $node->column = 1;
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $node->proprietary = true;
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $node->id = null;
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $node->attribute = [];
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $node->child = [];
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot modify readonly property tidyNode::$value
Error: Cannot modify readonly property tidyNode::$name
Error: Cannot modify readonly property tidyNode::$type
Error: Cannot modify readonly property tidyNode::$line
Error: Cannot modify readonly property tidyNode::$column
Error: Cannot modify readonly property tidyNode::$proprietary
Error: Cannot modify readonly property tidyNode::$id
Error: Cannot modify readonly property tidyNode::$attribute
Error: Cannot modify readonly property tidyNode::$child
