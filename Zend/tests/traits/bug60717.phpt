--TEST--
Bug #60717 (Order of traits in use statement can cause unexpected unresolved abstract method)
--FILE--
<?php

namespace HTML
{
    interface Helper
    {
        function text($text);
        function attributes(array $attributes = null);
        function textArea(?array $attributes, $value);
    }

    trait TextUTF8
    {
        function text($text) {}
    }

    trait TextArea
    {
        function textArea(?array $attributes, $value) {}
        abstract function attributes(array $attributes = null);
        abstract function text($text);
    }

    trait HTMLAttributes
    {
        function attributes(array $attributes = null) {	}
        abstract function text($text);
    }

    class HTMLHelper implements Helper
    {
        use TextArea, HTMLAttributes, TextUTF8;
    }

    class HTMLHelper2 implements Helper
    {
        use TextArea, TextUTF8, HTMLAttributes;
    }

    class HTMLHelper3 implements Helper
    {
        use HTMLAttributes, TextArea, TextUTF8;
    }

    class HTMLHelper4 implements Helper
    {
        use HTMLAttributes, TextUTF8, TextArea;
    }

    class HTMLHelper5 implements Helper
    {
        use TextUTF8, TextArea, HTMLAttributes;
    }

    class HTMLHelper6 implements Helper
    {
        use TextUTF8, HTMLAttributes, TextArea;
    }

    $o = new HTMLHelper;
    $o = new HTMLHelper2;
    $o = new HTMLHelper3;
    $o = new HTMLHelper4;
    $o = new HTMLHelper5;
    $o = new HTMLHelper6;
    echo 'Done';
}
?>
--EXPECT--
Done
