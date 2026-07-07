--TEST--
Markup syntax: the RFC's headline greeting() example (multi-line, whitespace-normalized)
--EXTENSIONS--
html
--FILE--
<?php
function greeting(string $name, string $type): Html\Htmlable
{
    return <>
        <h1 class="title">Hello, {$name ?: ucfirst($type)}!</h1>
        <p>Welcome to PHP, where markup is a first-class expression.</p>
    </>;
}

echo greeting('<Ada>', 'guest')->__toString(), "\n";
?>
--EXPECT--
<h1 class="title">Hello, &lt;Ada&gt;!</h1><p>Welcome to PHP, where markup is a first-class expression.</p>
