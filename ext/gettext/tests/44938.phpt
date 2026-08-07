--TEST--
#44938: gettext functions crash with overlong strings
--EXTENSIONS--
gettext
--FILE--
<?php
$overflown = str_repeat('C', 8476509);
$msgid     = "msgid";
$domain    = "domain";
$category  = "cat";

try {
    bindtextdomain($overflown, 'path');
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dngettext($overflown, $msgid, $msgid, 1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dngettext($domain, $overflown, $msgid, 1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dngettext($domain, $msgid, $overflown, 1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    gettext($overflown);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    ngettext($overflown, $msgid, -1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    ngettext($msgid, $overflown, -1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dcgettext($overflown, $msgid, -1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dcgettext($domain, $overflown, -1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dcngettext($overflown, $msgid, $msgid, -1, -1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dcngettext($domain, $overflown, $msgid, -1, -1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dcngettext($domain, $msgid, $overflown, -1, -1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dgettext($overflown, $msgid);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    dgettext($domain, $overflown);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    textdomain($overflown);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bindtextdomain(): Argument #1 ($domain) is too long
ValueError: dngettext(): Argument #1 ($domain) is too long
ValueError: dngettext(): Argument #2 ($singular) is too long
ValueError: dngettext(): Argument #3 ($plural) is too long
ValueError: gettext(): Argument #1 ($message) is too long
ValueError: ngettext(): Argument #1 ($singular) is too long
ValueError: ngettext(): Argument #2 ($plural) is too long
ValueError: dcgettext(): Argument #1 ($domain) is too long
ValueError: dcgettext(): Argument #2 ($message) is too long
ValueError: dcngettext(): Argument #1 ($domain) is too long
ValueError: dcngettext(): Argument #2 ($singular) is too long
ValueError: dcngettext(): Argument #3 ($plural) is too long
ValueError: dgettext(): Argument #1 ($domain) is too long
ValueError: dgettext(): Argument #2 ($message) is too long
ValueError: textdomain(): Argument #1 ($domain) is too long
