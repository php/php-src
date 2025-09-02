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
    echo $exception->getMessage() . "\n";
}

try {
    $node->name = "";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $node->type = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $node->line = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $node->column = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $node->proprietary = true;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $node->id = null;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $node->attribute = [];
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $node->child = [];
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify readonly property tidyNode::$value
Cannot modify readonly property tidyNode::$name
Cannot modify readonly property tidyNode::$type
Cannot modify readonly property tidyNode::$line
Cannot modify readonly property tidyNode::$column
Cannot modify readonly property tidyNode::$proprietary
Cannot modify readonly property tidyNode::$id
Cannot modify readonly property tidyNode::$attribute
Cannot modify readonly property tidyNode::$child
